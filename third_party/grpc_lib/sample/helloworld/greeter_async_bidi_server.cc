/*
 *
 * Copyright 2016, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "pch.h"

#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

#include <grpc++/grpc++.h>

#include "hellostreamingworld.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncReaderWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::CompletionQueue;
using grpc::ServerCompletionQueue;
using grpc::Status;
using hellostreamingworld::HelloRequest;
using hellostreamingworld::HelloReply;
using hellostreamingworld::MultiGreeter;

enum class Type { READ = 1, WRITE = 2, CONNECT = 3, FINISH = 4, DISCONNECT = 5, };

// NOTE: This is a complex example for an asynchronous, bidirectional streaming
// server. For a simpler example, start with the
// greeter_server/greeter_async_server first.

// Most of the logic is similar to AsyncBidiGreeterClient, so follow that class
// for detailed comments. The only difference between the server and the client
// is the (a) concept of 'listening' as well as (b) client 'Finish()' that
// closes a specific client->server stream (but lets the server handle multiple
// streams).
class AsyncBidiGreeterServer {
 public:
  AsyncBidiGreeterServer() {
    // In general avoid setting up the server in the main thread (specifically,
    // in a constructor-like function such as this). We ignore this in the
    // context of an example.
    std::string server_address("0.0.0.0:50051");

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);
    cq_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();

    Activate();

    LOGI << "Server listening on " << server_address;
  }

  void SetResponse(const std::string& response) { response_str_ = response; }

  ~AsyncBidiGreeterServer() {
    Dectivate();
  }

 private:
  void Activate() {
    WaitClientConnection();

    grpc_thread_.reset(new std::thread([=]()
    {
      LOGI << "Thread start...";
      GrpcThread();
      LOGI << "Thread end...";
    }));
  }

  void WaitClientConnection()
  {
    // This initiates a single stream for a single client. To allow multiple
    // clients in different threads to connect, simply 'request' from the
    // different threads. Each stream is independent but can use the same
    // completion queue/context objects.
    context_.reset(new ServerContext);
    stream_.reset(new ServerAsyncReaderWriter<HelloReply, HelloRequest>(context_.get()));
    new_cq_.reset(new CompletionQueue);
    context_->AsyncNotifyWhenDone(reinterpret_cast<void*>(Type::DISCONNECT));
    service_.RequestSayHelloDb(context_.get(), stream_.get(), new_cq_.get(), cq_.get(), reinterpret_cast<void*>(Type::CONNECT));

    void* got_tag = nullptr;
    bool ok = false;
    if (!cq_->Next(&got_tag, &ok)) {
        LOGI << "Client stream closed. Quitting";
    }
    if (static_cast<Type>(reinterpret_cast<size_t>(got_tag)) == Type::CONNECT)
    {
        LOGI << "Client connected.";
        AsyncWaitForHelloRequest();
    }
  }

  void Dectivate() {
    LOGI << "Shutting down server.";

    context_->TryCancel();
    server_->Shutdown();
    // Always shutdown the completion queue after the server.
    cq_->Shutdown();
    new_cq_->Shutdown();

    if (grpc_thread_->joinable())
      grpc_thread_->join();
  }

  void AsyncWaitForHelloRequest() {
    // In the case of the server, we wait for a READ first and then write a
    // response. A server cannot initiate a connection so the server has to
    // wait for the client to send a message in order for it to respond back.
    stream_->Read(&request_, reinterpret_cast<void*>(Type::READ));
  }

  void AsyncHelloSendResponse() {
    LOGI << " ** Handling request: " << request_.name();
    HelloReply response;
    LOGI << " ** Sending response: " << response_str_;
    response.set_message(response_str_);
    stream_->Write(response, reinterpret_cast<void*>(Type::WRITE));
  }

  void GrpcThread() {
    while (true) {
      void* got_tag = nullptr;
      bool ok = false;
      if (!new_cq_->Next(&got_tag, &ok)) {
        LOGI << "Client stream closed. Quitting";
        break;
      }

      if (ok) {
        LOGI << "**** Processing completion queue tag " << got_tag;
        switch (static_cast<Type>(reinterpret_cast<size_t>(got_tag))) {
          case Type::READ:
            LOGI << "Read a new message.";
            AsyncHelloSendResponse();
            break;
          case Type::WRITE:
            LOGI << "Sending message (async).";
            AsyncWaitForHelloRequest();
            break;
          case Type::CONNECT:
            LOGI << "Client connected.";
            AsyncWaitForHelloRequest();
            break;
          case Type::DISCONNECT:
            LOGI << "Client disconnected.";
            WaitClientConnection();
            break;
          default:
            LOGE << "Unexpected tag " << got_tag;
            GPR_ASSERT(false);
        }
      }
    }
  }

 private:
  HelloRequest request_;
  std::string response_str_ = "Default server response";
  std::unique_ptr<ServerContext> context_;
  std::unique_ptr<ServerCompletionQueue> cq_;
  std::unique_ptr<CompletionQueue> new_cq_;
  MultiGreeter::AsyncService service_;
  std::unique_ptr<Server> server_;
  std::unique_ptr<ServerAsyncReaderWriter<HelloReply, HelloRequest>> stream_;
  std::unique_ptr<std::thread> grpc_thread_;
};

int main(int argc, char** argv) {
  static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::verbose, &consoleAppender);

  AsyncBidiGreeterServer server;

  std::string response;
  while (true) {
    std::cout << "Enter next set of responses (type quit to end): ";
    std::cin >> response;
    if (response == "quit") {
      break;
    }
    server.SetResponse(response);
  }

  return 0;
}