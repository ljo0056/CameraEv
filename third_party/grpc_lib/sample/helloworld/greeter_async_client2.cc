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

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <thread>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;


class GreeterClient {
  public:
    explicit GreeterClient(std::shared_ptr<Channel> channel)
            : stub_(Greeter::NewStub(channel)) {}

    void Stop() {
      cq_.Shutdown();

      void* ignored_tag;
      bool ignored_ok;
      while (cq_.Next(&ignored_tag, &ignored_ok))
          ;
    }

    // Assembles the client's payload and sends it to the server.
    void SayHello(const std::string& user) {
        // Data we are sending to the server.
        HelloRequest request;
        request.set_name(user);

        // Call object to store rpc data
        AsyncClientCall* call = new AsyncClientCall;

        //std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
        //    std::chrono::milliseconds(1000);
        //call->context.set_deadline(deadline);

        // stub_->PrepareAsyncSayHello() creates an RPC object, returning
        // an instance to store in "call" but does not actually start the RPC
        // Because we are using the asynchronous API, we need to hold on to
        // the "call" instance in order to get updates on the ongoing RPC.
        call->response_reader = stub_->PrepareAsyncSayHello(&call->context, request, &cq_);

        // StartCall initiates the RPC call
        call->response_reader->StartCall();

        // Request that, upon completion of the RPC, "reply" be updated with the
        // server's response; "status" with the indication of whether the operation
        // was successful. Tag the request with the memory address of the call object.
        call->response_reader->Finish(&call->reply, &call->status, (void*)call);
        //LOGI << "send : " << request.name();
    }

    // Loop while listening for completed responses.
    // Prints out the response from the server.
    void AsyncCompleteRpc() {
        void* got_tag = nullptr;
        bool ok = false;

        // Block until the next result is available in the completion queue "cq".
        while (cq_.Next(&got_tag, &ok)) {
            // The tag in this example is the memory location of the call object
            AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

            // Verify that the request was completed successfully. Note that "ok"
            // corresponds solely to the request for updates introduced by Finish().
            GPR_ASSERT(ok);

            if (call->status.ok())
                LOGI << "Greeter received: " << call->reply.message();
            else
                LOGI << "RPC failed  message:" << call->status.error_message();

            // Once we're complete, deallocate the call object.
            delete call;
        }
    }

    void AsyncCompleteRpc2() {
        void* got_tag = nullptr;
        bool ok = false;
        bool shutdown = false;

        std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() +
            std::chrono::milliseconds(3000);

        while (true) {
            ::grpc::CompletionQueue::NextStatus ns = cq_.AsyncNext(&got_tag, &ok, deadline);

            // The tag in this example is the memory location of the call object
            AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

            switch (ns) {
            case ::grpc::CompletionQueue::SHUTDOWN:
                LOGI << "CompletionQueue::SHUTDOWN";
                shutdown = true;
                break;
            case ::grpc::CompletionQueue::GOT_EVENT:
                //LOGI << "CompletionQueue::GOT_EVENT";
                {
                    // Verify that the request was completed successfully. Note that "ok"
                    // corresponds solely to the request for updates introduced by Finish().
                    GPR_ASSERT(ok);

                    if (call->status.ok())
                        LOGI << "Greeter received: " << call->reply.message();
                    else
                        LOGI << "RPC failed";

                    // Once we're complete, deallocate the call object.
                    delete call;
                    call = nullptr;
                }
                break;
            case ::grpc::CompletionQueue::TIMEOUT:
                //LOGI << "CompletionQueue::TIMEOUT";
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                break;
            default:
                LOGI << "CompletionQueue::default";
                break;
            }

            if (shutdown)
                break;
        }
    }

  private:

    // struct for keeping state and data information
    struct AsyncClientCall {
        // Container for the data we expect from the server.
        HelloReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // Storage for the status of the RPC upon completion.
        Status status;

        std::unique_ptr<ClientAsyncResponseReader<HelloReply>> response_reader;
    };

    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<Greeter::Stub> stub_;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq_;

  public:
    const int send_count_ = 5;
};

int main(int argc, char** argv) {

    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);

    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint (in this case,
    // localhost at port 50051). We indicate that the channel isn't authenticated
    // (use of InsecureChannelCredentials()).
    GreeterClient greeter(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    LOGI << "quit input 'q' key...";

    // Spawn reader thread that loops indefinitely
    std::thread thread_ = std::thread([&greeter]() {
        LOGI << "thread start... ";
        greeter.AsyncCompleteRpc();
        LOGI << "thread end... ";
    });

    while (true) {
        char ch = getchar();
        if ('q' == ch) {
            LOGI << "'q' key interrupt";
            break;
        }
        else if ('s' == ch) {
            for (int i = 0; i < greeter.send_count_; i++) {
                std::string user("world " + std::to_string(i));
                greeter.SayHello(user);  // The actual RPC call!
            }
        }
    }

    greeter.Stop();

    if (thread_.joinable())
        thread_.join();

    return 0;
}
