// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: camera_control.proto
// Original file comments:
// Copyright 2015 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#ifndef GRPC_camera_5fcontrol_2eproto__INCLUDED
#define GRPC_camera_5fcontrol_2eproto__INCLUDED

#include "camera_control.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace camera_control {

// The CameraControl service definition.
class CameraControl final {
 public:
  static constexpr char const* service_full_name() {
    return "camera_control.CameraControl";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status Command(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::camera_control::Packet* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::camera_control::Packet>> AsyncCommand(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::camera_control::Packet>>(AsyncCommandRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::camera_control::Packet>> PrepareAsyncCommand(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::camera_control::Packet>>(PrepareAsyncCommandRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReaderInterface< ::camera_control::Packet>> VideoStream(::grpc::ClientContext* context, const ::camera_control::Packet& request) {
      return std::unique_ptr< ::grpc::ClientReaderInterface< ::camera_control::Packet>>(VideoStreamRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>> AsyncVideoStream(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>>(AsyncVideoStreamRaw(context, request, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>> PrepareAsyncVideoStream(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>>(PrepareAsyncVideoStreamRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReaderInterface< ::camera_control::Packet>> VideoAnalysis(::grpc::ClientContext* context, const ::camera_control::Packet& request) {
      return std::unique_ptr< ::grpc::ClientReaderInterface< ::camera_control::Packet>>(VideoAnalysisRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>> AsyncVideoAnalysis(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>>(AsyncVideoAnalysisRaw(context, request, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>> PrepareAsyncVideoAnalysis(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>>(PrepareAsyncVideoAnalysisRaw(context, request, cq));
    }
    class experimental_async_interface {
     public:
      virtual ~experimental_async_interface() {}
      virtual void Command(::grpc::ClientContext* context, const ::camera_control::Packet* request, ::camera_control::Packet* response, std::function<void(::grpc::Status)>) = 0;
      virtual void Command(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::camera_control::Packet* response, std::function<void(::grpc::Status)>) = 0;
      virtual void VideoStream(::grpc::ClientContext* context, ::camera_control::Packet* request, ::grpc::experimental::ClientReadReactor< ::camera_control::Packet>* reactor) = 0;
      virtual void VideoAnalysis(::grpc::ClientContext* context, ::camera_control::Packet* request, ::grpc::experimental::ClientReadReactor< ::camera_control::Packet>* reactor) = 0;
    };
    virtual class experimental_async_interface* experimental_async() { return nullptr; }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::camera_control::Packet>* AsyncCommandRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::camera_control::Packet>* PrepareAsyncCommandRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientReaderInterface< ::camera_control::Packet>* VideoStreamRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>* AsyncVideoStreamRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>* PrepareAsyncVideoStreamRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientReaderInterface< ::camera_control::Packet>* VideoAnalysisRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>* AsyncVideoAnalysisRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::camera_control::Packet>* PrepareAsyncVideoAnalysisRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status Command(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::camera_control::Packet* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::camera_control::Packet>> AsyncCommand(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::camera_control::Packet>>(AsyncCommandRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::camera_control::Packet>> PrepareAsyncCommand(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::camera_control::Packet>>(PrepareAsyncCommandRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReader< ::camera_control::Packet>> VideoStream(::grpc::ClientContext* context, const ::camera_control::Packet& request) {
      return std::unique_ptr< ::grpc::ClientReader< ::camera_control::Packet>>(VideoStreamRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::camera_control::Packet>> AsyncVideoStream(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::camera_control::Packet>>(AsyncVideoStreamRaw(context, request, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::camera_control::Packet>> PrepareAsyncVideoStream(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::camera_control::Packet>>(PrepareAsyncVideoStreamRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReader< ::camera_control::Packet>> VideoAnalysis(::grpc::ClientContext* context, const ::camera_control::Packet& request) {
      return std::unique_ptr< ::grpc::ClientReader< ::camera_control::Packet>>(VideoAnalysisRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::camera_control::Packet>> AsyncVideoAnalysis(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::camera_control::Packet>>(AsyncVideoAnalysisRaw(context, request, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::camera_control::Packet>> PrepareAsyncVideoAnalysis(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::camera_control::Packet>>(PrepareAsyncVideoAnalysisRaw(context, request, cq));
    }
    class experimental_async final :
      public StubInterface::experimental_async_interface {
     public:
      void Command(::grpc::ClientContext* context, const ::camera_control::Packet* request, ::camera_control::Packet* response, std::function<void(::grpc::Status)>) override;
      void Command(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::camera_control::Packet* response, std::function<void(::grpc::Status)>) override;
      void VideoStream(::grpc::ClientContext* context, ::camera_control::Packet* request, ::grpc::experimental::ClientReadReactor< ::camera_control::Packet>* reactor) override;
      void VideoAnalysis(::grpc::ClientContext* context, ::camera_control::Packet* request, ::grpc::experimental::ClientReadReactor< ::camera_control::Packet>* reactor) override;
     private:
      friend class Stub;
      explicit experimental_async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class experimental_async_interface* experimental_async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class experimental_async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::camera_control::Packet>* AsyncCommandRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::camera_control::Packet>* PrepareAsyncCommandRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientReader< ::camera_control::Packet>* VideoStreamRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request) override;
    ::grpc::ClientAsyncReader< ::camera_control::Packet>* AsyncVideoStreamRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncReader< ::camera_control::Packet>* PrepareAsyncVideoStreamRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientReader< ::camera_control::Packet>* VideoAnalysisRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request) override;
    ::grpc::ClientAsyncReader< ::camera_control::Packet>* AsyncVideoAnalysisRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncReader< ::camera_control::Packet>* PrepareAsyncVideoAnalysisRaw(::grpc::ClientContext* context, const ::camera_control::Packet& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_Command_;
    const ::grpc::internal::RpcMethod rpcmethod_VideoStream_;
    const ::grpc::internal::RpcMethod rpcmethod_VideoAnalysis_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status Command(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::camera_control::Packet* response);
    virtual ::grpc::Status VideoStream(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer);
    virtual ::grpc::Status VideoAnalysis(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer);
  };
  template <class BaseClass>
  class WithAsyncMethod_Command : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_Command() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_Command() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Command(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::camera_control::Packet* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestCommand(::grpc::ServerContext* context, ::camera_control::Packet* request, ::grpc::ServerAsyncResponseWriter< ::camera_control::Packet>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_VideoStream : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_VideoStream() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_VideoStream() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status VideoStream(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestVideoStream(::grpc::ServerContext* context, ::camera_control::Packet* request, ::grpc::ServerAsyncWriter< ::camera_control::Packet>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncServerStreaming(1, context, request, writer, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_VideoAnalysis : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_VideoAnalysis() {
      ::grpc::Service::MarkMethodAsync(2);
    }
    ~WithAsyncMethod_VideoAnalysis() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status VideoAnalysis(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestVideoAnalysis(::grpc::ServerContext* context, ::camera_control::Packet* request, ::grpc::ServerAsyncWriter< ::camera_control::Packet>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncServerStreaming(2, context, request, writer, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_Command<WithAsyncMethod_VideoStream<WithAsyncMethod_VideoAnalysis<Service > > > AsyncService;
  template <class BaseClass>
  class ExperimentalWithCallbackMethod_Command : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithCallbackMethod_Command() {
      ::grpc::Service::experimental().MarkMethodCallback(0,
        new ::grpc::internal::CallbackUnaryHandler< ::camera_control::Packet, ::camera_control::Packet>(
          [this](::grpc::ServerContext* context,
                 const ::camera_control::Packet* request,
                 ::camera_control::Packet* response,
                 ::grpc::experimental::ServerCallbackRpcController* controller) {
                   return this->Command(context, request, response, controller);
                 }));
    }
    ~ExperimentalWithCallbackMethod_Command() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Command(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::camera_control::Packet* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual void Command(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::camera_control::Packet* response, ::grpc::experimental::ServerCallbackRpcController* controller) { controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "")); }
  };
  template <class BaseClass>
  class ExperimentalWithCallbackMethod_VideoStream : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithCallbackMethod_VideoStream() {
      ::grpc::Service::experimental().MarkMethodCallback(1,
        new ::grpc::internal::CallbackServerStreamingHandler< ::camera_control::Packet, ::camera_control::Packet>(
          [this] { return this->VideoStream(); }));
    }
    ~ExperimentalWithCallbackMethod_VideoStream() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status VideoStream(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::experimental::ServerWriteReactor< ::camera_control::Packet, ::camera_control::Packet>* VideoStream() {
      return new ::grpc::internal::UnimplementedWriteReactor<
        ::camera_control::Packet, ::camera_control::Packet>;}
  };
  template <class BaseClass>
  class ExperimentalWithCallbackMethod_VideoAnalysis : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithCallbackMethod_VideoAnalysis() {
      ::grpc::Service::experimental().MarkMethodCallback(2,
        new ::grpc::internal::CallbackServerStreamingHandler< ::camera_control::Packet, ::camera_control::Packet>(
          [this] { return this->VideoAnalysis(); }));
    }
    ~ExperimentalWithCallbackMethod_VideoAnalysis() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status VideoAnalysis(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::experimental::ServerWriteReactor< ::camera_control::Packet, ::camera_control::Packet>* VideoAnalysis() {
      return new ::grpc::internal::UnimplementedWriteReactor<
        ::camera_control::Packet, ::camera_control::Packet>;}
  };
  typedef ExperimentalWithCallbackMethod_Command<ExperimentalWithCallbackMethod_VideoStream<ExperimentalWithCallbackMethod_VideoAnalysis<Service > > > ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_Command : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_Command() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_Command() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Command(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::camera_control::Packet* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_VideoStream : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_VideoStream() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_VideoStream() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status VideoStream(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_VideoAnalysis : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_VideoAnalysis() {
      ::grpc::Service::MarkMethodGeneric(2);
    }
    ~WithGenericMethod_VideoAnalysis() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status VideoAnalysis(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_Command : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_Command() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_Command() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Command(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::camera_control::Packet* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestCommand(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawMethod_VideoStream : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_VideoStream() {
      ::grpc::Service::MarkMethodRaw(1);
    }
    ~WithRawMethod_VideoStream() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status VideoStream(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestVideoStream(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncWriter< ::grpc::ByteBuffer>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncServerStreaming(1, context, request, writer, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawMethod_VideoAnalysis : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_VideoAnalysis() {
      ::grpc::Service::MarkMethodRaw(2);
    }
    ~WithRawMethod_VideoAnalysis() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status VideoAnalysis(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestVideoAnalysis(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncWriter< ::grpc::ByteBuffer>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncServerStreaming(2, context, request, writer, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class ExperimentalWithRawCallbackMethod_Command : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithRawCallbackMethod_Command() {
      ::grpc::Service::experimental().MarkMethodRawCallback(0,
        new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
          [this](::grpc::ServerContext* context,
                 const ::grpc::ByteBuffer* request,
                 ::grpc::ByteBuffer* response,
                 ::grpc::experimental::ServerCallbackRpcController* controller) {
                   this->Command(context, request, response, controller);
                 }));
    }
    ~ExperimentalWithRawCallbackMethod_Command() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Command(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::camera_control::Packet* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual void Command(::grpc::ServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response, ::grpc::experimental::ServerCallbackRpcController* controller) { controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "")); }
  };
  template <class BaseClass>
  class ExperimentalWithRawCallbackMethod_VideoStream : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithRawCallbackMethod_VideoStream() {
      ::grpc::Service::experimental().MarkMethodRawCallback(1,
        new ::grpc::internal::CallbackServerStreamingHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
          [this] { return this->VideoStream(); }));
    }
    ~ExperimentalWithRawCallbackMethod_VideoStream() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status VideoStream(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::experimental::ServerWriteReactor< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* VideoStream() {
      return new ::grpc::internal::UnimplementedWriteReactor<
        ::grpc::ByteBuffer, ::grpc::ByteBuffer>;}
  };
  template <class BaseClass>
  class ExperimentalWithRawCallbackMethod_VideoAnalysis : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithRawCallbackMethod_VideoAnalysis() {
      ::grpc::Service::experimental().MarkMethodRawCallback(2,
        new ::grpc::internal::CallbackServerStreamingHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
          [this] { return this->VideoAnalysis(); }));
    }
    ~ExperimentalWithRawCallbackMethod_VideoAnalysis() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status VideoAnalysis(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::experimental::ServerWriteReactor< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* VideoAnalysis() {
      return new ::grpc::internal::UnimplementedWriteReactor<
        ::grpc::ByteBuffer, ::grpc::ByteBuffer>;}
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_Command : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_Command() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler< ::camera_control::Packet, ::camera_control::Packet>(std::bind(&WithStreamedUnaryMethod_Command<BaseClass>::StreamedCommand, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_Command() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status Command(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::camera_control::Packet* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedCommand(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::camera_control::Packet,::camera_control::Packet>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_Command<Service > StreamedUnaryService;
  template <class BaseClass>
  class WithSplitStreamingMethod_VideoStream : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithSplitStreamingMethod_VideoStream() {
      ::grpc::Service::MarkMethodStreamed(1,
        new ::grpc::internal::SplitServerStreamingHandler< ::camera_control::Packet, ::camera_control::Packet>(std::bind(&WithSplitStreamingMethod_VideoStream<BaseClass>::StreamedVideoStream, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithSplitStreamingMethod_VideoStream() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status VideoStream(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with split streamed
    virtual ::grpc::Status StreamedVideoStream(::grpc::ServerContext* context, ::grpc::ServerSplitStreamer< ::camera_control::Packet,::camera_control::Packet>* server_split_streamer) = 0;
  };
  template <class BaseClass>
  class WithSplitStreamingMethod_VideoAnalysis : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithSplitStreamingMethod_VideoAnalysis() {
      ::grpc::Service::MarkMethodStreamed(2,
        new ::grpc::internal::SplitServerStreamingHandler< ::camera_control::Packet, ::camera_control::Packet>(std::bind(&WithSplitStreamingMethod_VideoAnalysis<BaseClass>::StreamedVideoAnalysis, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithSplitStreamingMethod_VideoAnalysis() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status VideoAnalysis(::grpc::ServerContext* context, const ::camera_control::Packet* request, ::grpc::ServerWriter< ::camera_control::Packet>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with split streamed
    virtual ::grpc::Status StreamedVideoAnalysis(::grpc::ServerContext* context, ::grpc::ServerSplitStreamer< ::camera_control::Packet,::camera_control::Packet>* server_split_streamer) = 0;
  };
  typedef WithSplitStreamingMethod_VideoStream<WithSplitStreamingMethod_VideoAnalysis<Service > > SplitStreamedService;
  typedef WithStreamedUnaryMethod_Command<WithSplitStreamingMethod_VideoStream<WithSplitStreamingMethod_VideoAnalysis<Service > > > StreamedService;
};

}  // namespace camera_control


#endif  // GRPC_camera_5fcontrol_2eproto__INCLUDED
