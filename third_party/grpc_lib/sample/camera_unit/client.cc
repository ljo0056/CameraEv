
#include "pch.h"
#include <plog/Appenders/ColorConsoleAppender.h>
#include "PLogCommon.h"
#include "grpc_define.h"
#include "FPSChecker.h"

// c++ stl
#include <memory>
#include <string>

// grpc
#include <grpcpp/grpcpp.h>
#include "camera_control.grpc.pb.h"


#pragma warning(push)
#pragma warning(disable:4311)
#pragma warning(disable:4302)

//////////////////////////////////////////////////////////////////////////
// plog

namespace plog
{
    Record& operator<<(Record& record, const grpc::Status& value)
    {
        return record << "code["<< value.error_code() << "] " << "message[" << value.error_message() << "]";
    }
}

//////////////////////////////////////////////////////////////////////////

class gRpcClientImpl
{
private:
    typedef camera_control::CameraControl               GCameraControl;
    typedef camera_control::Packet                      GPacket;

    typedef grpc::Status                                GStatus;
    typedef grpc::Channel                               GChannel;
    typedef grpc::ChannelArguments                      GChannelArguments;
    typedef grpc::ClientContext                         GClientContext;
    typedef grpc::CompletionQueue                       GCompletionQueue;

    typedef grpc::ClientAsyncResponseReader<GPacket>    GAsyncReader;
    typedef grpc::ClientAsyncReader<GPacket>            GAsyncStreamReader;
    typedef grpc::ClientAsyncWriter<GPacket>            GAsyncStreamWriter;

    std::thread                             m_thread;

    std::string                             m_url;
    GStatus                                 m_status;
    GClientContext                          m_command_ctx;
    GCompletionQueue                        m_completion_queue;

    std::unique_ptr<GAsyncStreamReader>     m_stream_reader;
    std::unique_ptr<GAsyncStreamReader>     m_va_metadata_reader;

    std::unique_ptr<GCameraControl::Stub>   m_stub;
    std::shared_ptr<GChannel>               m_channel;
    FPSChecker                              m_fps_checker;

    enum STATUS
    {
        NONE = 0,
        CONNECTION,
        STREAM_READ,
        FINISH,
    };

    enum 
    {
        BUFFER_LEN = 1024,
    };

    std::array<uint8_t, BUFFER_LEN>         m_buffer;

private:
    void* Tag(STATUS tag)
    {
        return reinterpret_cast<void*>(tag);
    }

    STATUS Detag(void* tag)
    {
        return static_cast<STATUS>(reinterpret_cast<int>(tag));
    }

    std::chrono::system_clock::time_point GetDeadline(int ms)
    {
        return std::chrono::system_clock::now() + std::chrono::milliseconds(ms);
    }

    int SendCommand(int timeout_ms = 300)
    {
        GClientContext ctx;

        GPacket grpc_request, grpc_response;
        grpc_request.set_name("VideoStream");
        grpc_request.set_data(m_buffer.data(), (int)m_buffer.size());

        ctx.set_deadline(GetDeadline(timeout_ms));
        GStatus grpc_status = m_stub->Command(&ctx, grpc_request, &grpc_response);
        if (false == grpc_status.ok())
        {
            LOGE << "Send command failed  " << grpc_status;
            return 1;
        }

        // Check response
        LOGI << "Recv command name[" << grpc_response.name() << "]";
        LOGI << "Recv command data len[" << grpc_response.data().size() << "]";

        return 0;
    }

    int ProcWork()
    {
        void*   got_tag = nullptr;
        bool    ok = false, finish = false;
        GPacket grpc_response;

        m_stream_reader->Read(&grpc_response, Tag(STATUS::STREAM_READ));    // notiy read start 

        while (m_completion_queue.Next(&got_tag, &ok))
        {
            STATUS tag_status = Detag(got_tag);

            switch (tag_status)
            {
            case STATUS::CONNECTION:
                LOGI << "Stream connection...";
                break;
            case STATUS::STREAM_READ:
                if (ok)
                {
                    m_stream_reader->Read(&grpc_response, Tag(STATUS::STREAM_READ));

                    //LOGI << "Recv video stream name[" << grpc_response.name() << "]";
                    //LOGI << "Recv video stream data len[" << grpc_response.data().size() << "]";

                    m_fps_checker.DoCheckFPS();
                }
                break;
            case STATUS::FINISH:
                LOGI << "Stream finish..." << m_status;
                finish = true;
                break;
            default:
                break;
            }

            if (finish)
            {
                // loop 탈출
                // note : m_status 의 코드 값이 "UNIMPLEMENTED" 이라면 아직 서버의 Service 가 실행되고 있지 않은 상태이다.
                break;
            }
        }

        return 0;
    }

public:
    gRpcClientImpl()
    {
        
    }

    ~gRpcClientImpl()
    {

    }

    int Initialize(const std::string& url)
    {
        m_url = url;
        std::fill(m_buffer.begin(), m_buffer.end(), 0);
        
        GChannelArguments args;
        args.SetMaxReceiveMessageSize(GRPC_CLIENT_RECV_BUFFER_SIZE);

        m_channel = grpc::CreateCustomChannel(url, grpc::InsecureChannelCredentials(), args);
        m_stub    = GCameraControl::NewStub(m_channel);

        m_fps_checker.SetCallback([this](double ms, int count)
        {
            logi("Stream  fps   Time[%.1f] count[%d]", ms, count);
        });

        return 0;
    }

    int Finalize()
    {
        m_stub.reset();
        m_channel.reset();

        return 0;
    }

    int Activate()
    {
        GPacket grpc_request;
        grpc_request.set_name("VideoStream");
        grpc_request.set_data(m_buffer.data(), (int)m_buffer.size());

        m_stream_reader = m_stub->PrepareAsyncVideoStream(&m_command_ctx, grpc_request, &m_completion_queue);
        m_stream_reader->StartCall(Tag(STATUS::CONNECTION));

        void* got_tag = nullptr;
        bool  ok = false;

        if (false == m_completion_queue.Next(&got_tag, &ok))
        {
            LOGE << "Completion queue Next failed";
            return 1;
        }

        if (false == ok || STATUS::CONNECTION != Detag(got_tag))
        {
            LOGE << "Completion queue failed  ok[" << ok << "] " << "tag[" << Detag(got_tag) << "]";
            return 2;
        }

        GPacket grpc_response;
        m_stream_reader->Finish(&m_status, Tag(STATUS::FINISH));

        m_thread = std::thread([this]()
        {
            ProcWork();
        });

        return 0;
    }

    int Deactivate()
    {
        m_completion_queue.Shutdown();  // m_completion_queue.Next 에서 false 를 return 함.

        if (m_thread.joinable())
            m_thread.join();

        return 0;
    }
};

#pragma warning(pop)

//////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) 
{
    static plog::ColorConsoleAppender<plog::CCommonFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);

    LOGI << "Client main start...";

    std::vector<std::unique_ptr<gRpcClientImpl>> vec_impl;
    for (int ii = 0; ii < 4; ii++)
    {
        vec_impl.push_back(std::unique_ptr<gRpcClientImpl>(new gRpcClientImpl));
    }

    char ip_address[128] = { 0, };
    int port = 50051;
    do 
    {
        for (std::unique_ptr<gRpcClientImpl>& impl : vec_impl)
        {
            sprintf_s(ip_address, "localhost:%d", port++);
            impl->Initialize(ip_address);
            if (impl->Activate())
                break;
        }

        getchar();

    } while (0);

    for (std::unique_ptr<gRpcClientImpl>& impl : vec_impl)
    {
        impl->Deactivate();
        impl->Finalize();
    }
    vec_impl.clear();

    LOGI << "Client main end...";

    return 0;
}
