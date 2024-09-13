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

#include <grpc++/grpc++.h>

#include "hellostreamingworld.grpc.pb.h"

using grpc::Channel;
using grpc::ClientAsyncReaderWriter;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using hellostreamingworld::HelloRequest;
using hellostreamingworld::HelloReply;
using hellostreamingworld::MultiGreeter;

// NOTE: This is a complex example for an asynchronous, bidirectional streaming
// client. For a simpler example, start with the
// greeter_client/greeter_async_client first.
class AsyncBidiGreeterClient {
  enum class Type { READ = 1, WRITE = 2, CONNECT = 3, DISCONNECT = 4, };

 public:
  explicit AsyncBidiGreeterClient(std::shared_ptr<Channel> channel)
      : stub_(MultiGreeter::NewStub(channel))
      , channel_(channel) {
    grpc_thread_.reset(new std::thread([=]() {
        GrpcThread(); 
    }));
    
    stream_ = stub_->AsyncSayHelloDb(&context_, &cq_, reinterpret_cast<void*>(Type::CONNECT));
  }

  // Similar to the async hello example in greeter_async_client but does not
  // wait for the response. Instead queues up a tag in the completion queue
  // that is notified when the server responds back (or when the stream is
  // closed).
  void AsyncSayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // This is important: You can have at most one write or at most one read
    // at any given time. The throttling is performed by gRPC completion
    // queue. If you queue more than one write/read, the stream will crash.
    // Because this stream is bidirectional, you *can* have a single read
    // and a single write request queued for the same stream. Writes and reads
    // are independent of each other in terms of ordering/delivery.
    LOGI << " ** Sending request: " << user;
    stream_->Write(request, reinterpret_cast<void*>(Type::WRITE));
  }

  ~AsyncBidiGreeterClient() {
    LOGI << "Shutting down client.";
    grpc::Status status;
    cq_.Shutdown();
    grpc_thread_->join();
  }

 private:
  void AsyncHelloRequestNextMessage() {
    // The tag is the link between our thread (main thread) and the completion
    // queue thread. The tag allows the completion queue to fan off
    // notification handlers for the specified read/write requests as they
    // are being processed by gRPC.
    stream_->Read(&response_, reinterpret_cast<void*>(Type::READ));
  }

  // Runs a gRPC completion-queue processing thread. Checks for 'Next' tag
  // and processes them until there are no more (or when the completion queue
  // is shutdown).
  void GrpcThread() {
    while (true) {
      void* got_tag;
      bool ok = false;
      // Block until the next result is available in the completion queue "cq".
      // The return value of Next should always be checked. This return value
      // tells us whether there is any kind of event or the cq_ is shutting
      // down.
      if (!cq_.Next(&got_tag, &ok)) {
        LOGI << "Client stream closed. Quitting";
        break;
      }

      // It's important to process all tags even if the ok is false. One might
      // want to deallocate memory that has be reinterpret_cast'ed to void*
      // when the tag got initialized. For our example, we cast an int to a
      // void*, so we don't have extra memory management to take care of.
      if (ok) {
        LOGI << "**** Processing completion queue tag " << got_tag;
        switch (static_cast<Type>(reinterpret_cast<long>(got_tag))) {
          case Type::READ:
            LOGI << "Read a new message.";
            LOGI << " ** Got response: " << response_.message();
            break;
          case Type::WRITE:
            LOGI << "Sending message (async).";
            AsyncHelloRequestNextMessage();
            break;
          case Type::CONNECT:
            LOGI << "Server connected.";
            break;
          default:
            std::cerr << "Unexpected tag " << got_tag;
            GPR_ASSERT(false);
        }
      }
      else {
          LOGI << "cq failed..." << channel_->GetState(false);
      }
    }
  }

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context_;

  // The producer-consumer queue we use to communicate asynchronously with the
  // gRPC runtime.
  CompletionQueue cq_;

  // Out of the passed in Channel comes the stub, stored here, our view of the
  // server's exposed services.
  std::unique_ptr<MultiGreeter::Stub> stub_;
  std::shared_ptr<Channel> channel_;

  // The bidirectional, asynchronous stream for sending/receiving messages.
  std::unique_ptr<ClientAsyncReaderWriter<HelloRequest, HelloReply>> stream_;

  // Allocated protobuf that holds the response. In real clients and servers,
  // the memory management would a bit more complex as the thread that fills
  // in the response should take care of concurrency as well as memory
  // management.
  HelloReply response_;

  // Thread that notifies the gRPC completion queue tags.
  std::unique_ptr<std::thread> grpc_thread_;
};

int main(int argc, char** argv) {
  static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::verbose, &consoleAppender);

  AsyncBidiGreeterClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));

  std::string user;
  while (true) {
    std::cout << "Enter text (type quit to end): ";
    std::cin >> user;
    if (user == "quit") {
      break;
    }

    // Async RPC call that sends a message and awaits a response.
    greeter.AsyncSayHello(user);
  }
  return 0;
}