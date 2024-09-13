

#include "pch.h"
#include <plog/Appenders/ColorConsoleAppender.h>
#include "PLogCommon.h"
#include "FPSChecker.h"
#include "Locker.h"
#include "grpc_define.h"

#include <conio.h>

// c++ stl
#include <memory>
#include <string>

// grpc
#include <grpcpp/grpcpp.h>
#include "camera_control.grpc.pb.h"

using grpc::Status;
using grpc::Server;
using grpc::ServerContext;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::CompletionQueue;
using grpc::ServerAsyncWriter;
using grpc::ServerAsyncReader;
using grpc::ServerAsyncReaderWriter;
using grpc::ServerWriter;
using grpc::ServerReader;

using camera_control::CameraControl;

typedef camera_control::Packet GPacket;

//////////////////////////////////////////////////////////////////////////

void* Tag(int tag)
{
    return reinterpret_cast<void*>(tag);
}

int Detag(void* tag)
{
    return static_cast<int>(reinterpret_cast<int>(tag));
}

//////////////////////////////////////////////////////////////////////////
// plog

namespace plog
{
    Record& operator<<(Record& record, const grpc::Status& value)
    {
        return record << "code[" << value.error_code() << "] " << "message[" << value.error_message() << "]";
    }
}

//////////////////////////////////////////////////////////////////////////

class ClientSession
{
private:
    int                                 m_id = 0;
    std::thread                         m_thread;
    Locker                              m_locker_writer;
    FPSChecker                          m_fps_checker;

    CameraControl::AsyncService*        m_async_service;
    ServerCompletionQueue*              m_server_cq;

    std::unique_ptr<ServerContext>      m_ctx;
    std::unique_ptr<ServerAsyncWriter<GPacket>>  m_writer;

    GPacket                             m_request;
    CompletionQueue                     m_cq;
    Status                              m_grpc_status;

    int                                 m_status = DISCONNECT;

public:
    enum
    {
        DISCONNECT = 0,
        CONNECT,
        WRITE,
        FINISH,
    };

    ClientSession(int id, CameraControl::AsyncService* service, ServerCompletionQueue* server_cq)
        : m_id(id)
        , m_async_service(service)
        , m_server_cq(server_cq)
    {
        m_fps_checker.SetCallback([this](double ms, int count)
        {
            logi("grpc  fps   id[%d] Time[%.1f] count[%d]", m_id, ms, count);
        });
    }

    ~ClientSession()
    {

    }

    int GetId()
    {
        return m_id;
    }

    void WaitForConnect()
    {
        m_ctx.reset(new ServerContext);
        m_writer.reset(new ServerAsyncWriter<GPacket>(m_ctx.get()));

        // Client 가 접속 종료 시에 m_cq 에 Notify 를 할 수 있도록 설정
        m_ctx->AsyncNotifyWhenDone(Tag(DISCONNECT));

        // Client 의 VideoStream 요청에 대한 Notify 은 m_server_cq 에서 이루어짐
        // m_writer 이용하여 데이터를 쓰게 되면 Notify 은 m_cq 에서 이루어짐
        m_async_service->RequestVideoStream(m_ctx.get(), &m_request, m_writer.get(), &m_cq, m_server_cq, this);
    }

    void SendVideoStream(const GPacket& packet)
    {
        if (CONNECT == m_status && m_writer)
        {
            // Client 에 데이터를 전송 시에 m_cq 에 Write 완료 통지를 받아야지만
            // Clinet 에게 데이터를 전송 할 수 있는 상태가 된다.
            // 그에 대한 동기화를 m_locker_writer 를 이용한다.
            m_locker_writer.wait();
            m_writer->Write(packet, Tag(WRITE));
        }
    }

    int Activate()
    {
        if (m_thread.joinable())
            m_thread.join();

        m_thread = std::thread([this]()
        {
            m_status = CONNECT;
            m_locker_writer.wakeup();
            ProcWork();
            m_status = DISCONNECT;
        });

        return 0;
    }

    int Deactivate()
    {
        if (CONNECT == m_status && m_writer)
            m_writer->Finish(m_grpc_status, Tag(FINISH));   // 영상 데이터의 전송이 종료되었음을 Client 에게 통지 한다.

        m_cq.Shutdown();

        if (m_thread.joinable())
            m_thread.join();

        return 0;
    }

    void ProcWork()
    {
        LOGI << "ClientSession ProcWork start  id : " << m_id;
        bool out = false;

        while (true)
        {
            void* got_tag = nullptr;
            bool  ok = false;

            if (!m_cq.Next(&got_tag, &ok))
            {
                logi("Client stream closed.... id[%d]", m_id);
                break;
            }

            if (ok)
            {
                switch (Detag(got_tag))
                {
                case WRITE:
                    // 데이터 전송 완료 통지를 받았다. 다시 데이터 전송이 가능하도록 
                    // m_locker_writer 를 이용해서 동기화 시켜준다.
                    m_locker_writer.wakeup();
                    m_fps_checker.DoCheckFPS();
                    break;
                case FINISH:
                    logi("Clinet FINISH... id[%d] [%d:%s]", m_id, m_grpc_status.error_code(), m_grpc_status.error_message().c_str());
                    break;
                case DISCONNECT:
                    logi("Clinet DISCONNECT... id[%d]", m_id);
                    m_status = DISCONNECT;
                    WaitForConnect();   // Client 의 접속 종료 통지를 받았을 시에 다시 접속 대기 상태로 변경 한다.
                    out = true;
                    break;
                default:
                    logi("Clinet out of range... id[%d]", m_id);
                    break;
                }
            }
            else
            {
                logi("Clinet cq not ok... id[%d] tag[%d]", m_id, Detag(got_tag));
            }

            if (out)
                break;
        }

        LOGI << "ClientSession ProcWork end  id : " << m_id;
    }
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// class Server_grpc

class gRpcServerImpl
{
private:
    std::thread                                     m_thread;

    std::string                                     m_url;
    std::unique_ptr<CameraControl::AsyncService>    m_async_service;
    std::unique_ptr<grpc::Server>                   m_server;
    std::unique_ptr<grpc::ServerCompletionQueue>    m_server_cq;

    int                                             m_clinet_max_count = 10;
    std::vector<std::unique_ptr<ClientSession>>     m_clients;

    std::vector<uint8_t>                            m_video_stream_buffer;

private:
    void ProcWork()
    {
        LOGI << "Server ProcWork start";

        while (true)
        {
            void* got_tag = nullptr;
            bool  ok = false;

            // Client 에 대한 접속의 변화기 있는지 체크 한다.
            if (!m_server_cq->Next(&got_tag, &ok))
            {
                LOGI << "Client stream closed. Quitting";
                break;
            }

            if (ok)
            {
                ClientSession* clinet = static_cast<ClientSession*>(got_tag);

                LOGI << "Connect client... id : " << clinet->GetId();
                clinet->Activate();
            }
            else
            {
                LOGI << "Server cq not ok... : " << got_tag;
            }
        }

        LOGI << "Server ProcWork end";
    }

    uint8_t* AllocateVideoStreamBuffer(int size)
    {
        if (m_video_stream_buffer.size() == size)
            return m_video_stream_buffer.data();

        m_video_stream_buffer.resize(size);
        std::fill(m_video_stream_buffer.begin(), m_video_stream_buffer.end(), 0);

        return m_video_stream_buffer.data();
    }

public:
    gRpcServerImpl()
    {

    }

    virtual ~gRpcServerImpl()
    {

    }

    int Initialize(const std::string& url)
    {
        m_url = url;
        LOGI << "grpc url : " << m_url;

        m_async_service.reset(new CameraControl::AsyncService);

        ServerBuilder builder;
        builder.AddListeningPort(m_url, grpc::InsecureServerCredentials());
        builder.RegisterService(m_async_service.get());
        m_server_cq = builder.AddCompletionQueue();
        m_server = builder.BuildAndStart();
        if (nullptr == m_server)
            return 1;

        for (int ii = 0; ii < m_clinet_max_count; ii++)
        {
            std::unique_ptr<ClientSession> client(new ClientSession(ii, m_async_service.get(), m_server_cq.get()));
            m_clients.push_back(std::move(client));
        }

        return 0;
    }

    int Finalize()
    {
        m_clients.clear();

        return 0;
    }

    int Activate()
    {
        LOGI << "Server Activate";

        for (std::unique_ptr<ClientSession>& client : m_clients)
            client->WaitForConnect();

        m_thread = std::thread([this]()
        {
            ProcWork();
        });

        return 0;
    }

    int Deactivate()
    {
        LOGI << "Server Deactivate Start";

        for (std::unique_ptr<ClientSession>& client : m_clients)
            client->Deactivate();

        m_server->Shutdown();
        m_server_cq->Shutdown();

        if (m_thread.joinable())
            m_thread.join();

        LOGI << "Server Deactivate End";

        return 0;
    }

    int PushVideoStream(int image_len)
    {
        if (m_clients.empty())
            return 1;

        int buffer_len = image_len;
        AllocateVideoStreamBuffer(buffer_len);

        GPacket packet_video_stream;
        packet_video_stream.set_name("VIDEO_STREAM");
        packet_video_stream.set_data(m_video_stream_buffer.data(), buffer_len);

        for (std::unique_ptr<ClientSession>& client : m_clients)
            client->SendVideoStream(packet_video_stream);

        return 0;
    }
};

//////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    static plog::ColorConsoleAppender<plog::CCommonFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);

    LOGI << "Server main start... ";

    std::unique_ptr<gRpcServerImpl> impl(new gRpcServerImpl);

    std::string str_port;
    LOGI << "Port : ";
    std::cin >> str_port;

    std::string url = "localhost:" + str_port;
    impl->Initialize(url);
    impl->Activate();

    do
    {
        impl->PushVideoStream(1920 * 1080 * 3); // 실제 이미지를 전송할 시에 사이즈를 설정
        std::this_thread::sleep_for(std::chrono::milliseconds(30));  // 30 fps

        if (_kbhit())
        {
            char ch = tolower(getchar());
            if ('q' == ch)
            {
                LOGI << "Input quit key...";
                break;
            }
        }

    } while (true);

    impl->Deactivate();
    impl->Finalize();

    LOGI << "Server main end... ";;

    return 0;
}
