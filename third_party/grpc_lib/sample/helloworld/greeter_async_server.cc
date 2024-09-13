/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "pch.h"

#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <memory>
#include <iostream>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

class CustomGlobalCallbacks : public Server::GlobalCallbacks {
public:
    virtual ~CustomGlobalCallbacks() {}
    /// Called before server is created.
    virtual void UpdateArguments(grpc::ChannelArguments* args)
    {
        LOGI << "UpdateArguments... ";
    }
    /// Called before application callback for each synchronous server request
    virtual void PreSynchronousRequest(ServerContext* context)
    {
        LOGI << "PreSynchronousRequest... ";
    }
    /// Called after application callback for each synchronous server request
    virtual void PostSynchronousRequest(ServerContext* context)
    {
        LOGI << "PostSynchronousRequest... ";
    }
    /// Called before server is started.
    virtual void PreServerStart(Server* server)
    {
        LOGI << "PreServerStart... ";
    }
    /// Called after a server port is added.
    virtual void AddPort(Server* server, const grpc::string& addr, grpc::ServerCredentials* creds, int port)
    {
        LOGI << "AddPort... " << addr << " " << port;
    }
};

class ServerImpl final {
 public:
  ~ServerImpl() {
    
  }

  void Stop()
  {
      server_->Shutdown();
      // Always shutdown the completion queue after the server.
      cq_->Shutdown();
  }

  // There is no shutdown handling in this code.
  void Run() {
    std::string server_address("0.0.0.0:50051");

    Server::SetGlobalCallbacks(new CustomGlobalCallbacks);

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service_" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *asynchronous* service.
    builder.RegisterService(&service_);
    // Get hold of the completion queue used for the asynchronous communication
    // with the gRPC runtime.
    cq_ = builder.AddCompletionQueue();
    // Finally assemble the server.
    server_ = builder.BuildAndStart();
    LOGI << "Server listening on " << server_address;

    // Proceed to the server's main loop.
    HandleRpcs();
  }

 private:
  // Class encompasing the state and logic needed to serve a request.
  class CallData {
   public:
    // Take in the "service" instance (in this case representing an asynchronous
    // server) and the completion queue "cq" used for asynchronous communication
    // with the gRPC runtime.
    CallData(Greeter::AsyncService* service, ServerCompletionQueue* cq)
        : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
      // Invoke the serving logic right away.
      //LOGI << "CallData  address[0x" << this << "]";
      //ctx_.AsyncNotifyWhenDone((void*)100);
      Proceed();
      
    }
    ~CallData()
    {
      //LOGI << "~CallData  address[0x" << this << "]";
    }

    void Proceed() {
      if (status_ == CREATE) {
        // Make this instance progress to the PROCESS state.
        status_ = PROCESS;

        // As part of the initial CREATE state, we *request* that the system
        // start processing SayHello requests. In this request, "this" acts are
        // the tag uniquely identifying the request (so that different CallData
        // instances can serve different requests concurrently), in this case
        // the memory address of this CallData instance.
        service_->RequestSayHello(&ctx_, &request_, &responder_, cq_, cq_, this);
      } else if (status_ == PROCESS) {
        // Spawn a new CallData instance to serve new clients while we process
        // the one for this CallData. The instance will deallocate itself as
        // part of its FINISH state.
        CallData* calldata = new CallData(service_, cq_);

        // The actual processing.
        std::string prefix("Hello ");
        reply_.set_message(prefix + request_.name());
        calldata->name_ = reply_.message();

        // And we are done! Let the gRPC runtime know we've finished, using the
        // memory address of this instance as the uniquely identifying tag for
        // the event.
        status_ = FINISH;
        responder_.Finish(reply_, Status::OK, this);

        static int count = 0;
        LOGI << "send message [" << reply_ .message() << "]" << " count[" << count++ << "] " << name_;
      } else {
        GPR_ASSERT(status_ == FINISH);
        // Once in the FINISH state, deallocate ourselves (CallData).
        LOGI << "FINISH state : " << name_;
        delete this;
      }
    }

   private:
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    Greeter::AsyncService* service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue* cq_;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext ctx_;

    // What we get from the client.
    HelloRequest request_;
    // What we send back to the client.
    HelloReply reply_;

    // The means to get back to the client.
    ServerAsyncResponseWriter<HelloReply> responder_;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_;  // The current serving state.

   public:
    std::string name_;
  };

  // This can be run in multiple threads if needed.
  void HandleRpcs() {
    // Spawn a new CallData instance to serve new clients.
    CallData* callbada = new CallData(&service_, cq_.get());
    callbada->name_ = "main";

    void* tag;  // uniquely identifies a request.
    bool ok;
    while (true) {
      // Block waiting to read the next event from the completion queue. The
      // event is uniquely identified by its tag, which in this case is the
      // memory address of a CallData instance.
      // The return value of Next should always be checked. This return value
      // tells us whether there is any kind of event or cq_ is shutting down.
      GPR_ASSERT(cq_->Next(&tag, &ok));
      //GPR_ASSERT(ok);
      if (false == ok)
      {
        break;
      }

      CallData* calldata = static_cast<CallData*>(tag);
      calldata->Proceed();
    }
  }

  std::unique_ptr<ServerCompletionQueue> cq_;
  Greeter::AsyncService service_;
  std::unique_ptr<Server> server_;
};


int main(int argc, char** argv) {

    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);

    LOGI << "Server start... ";

    ServerImpl server;

    std::thread thread = std::thread([&server]() {
        LOGI << "thread start... ";
        server.Run();
        LOGI << "thread end... ";
    });

    LOGI << "quit input 'q' key...";
    while (true)
    {
        char ch = getchar();
        if ('q' == ch)
        {
            LOGI << "'q' key interrupt";
            break;
        }
    }

    LOGI << "Server Shutdown ";
    server.Stop();

    if (thread.joinable())
        thread.join();

    LOGI << "Server end... ";

  return 0;
}

