#include "stdafx.h"
#include "Server_grpc.h"
#include "PacketDefine.h"

#include "common/Locker.h"
#include "common/FPSChecker.h"

// c++ stl
#include <memory>
#include <string>

// grpc
#include <grpcpp/grpcpp.h>
#include "grpc/grpc_define.h"
#include "grpc/camera_control.grpc.pb.h"

using grpc::Status;
using grpc::Server;
using grpc::ServerContext;
using grpc::ServerBuilder;
using grpc::CompletionQueue;
using grpc::ServerCompletionQueue;
using grpc::ServerAsyncWriter;
using grpc::ServerAsyncReader;
using grpc::ServerAsyncReaderWriter;
using grpc::ServerWriter;
using grpc::ServerReader;

using camera_control::CameraControl;

typedef camera_control::Packet GPacket;


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

class CameraControlAsyncSevice : public CameraControl::AsyncService
{
public:
    CameraControlAsyncSevice()
        : CameraControl::AsyncService()
    {

    }

    virtual ~CameraControlAsyncSevice()
    {

    }
};

//////////////////////////////////////////////////////////////////////////

class ClientSession
{
private:
    int                         m_id = 0;
    std::thread                 m_thread;
    Locker                      m_locker_writer;
    FPSChecker                  m_fps_checker;

    ServerContext               m_ctx;
    GPacket                     m_request;
    ServerAsyncWriter<GPacket>  m_writer;
    CompletionQueue             m_cq;
    Status                      m_grpc_status;

    int                         m_status = DISCONNECT;

public:
    enum 
    {
        DISCONNECT = 0,
        CONNECT,
        WRITE,
        FINISH,
    };

    ClientSession(int id)
        : m_writer(&m_ctx)
        , m_id(id)
    {
        m_fps_checker.SetCallback([this](double ms, int count)
        {
            LOGI << fmt::format("grpc  fps   id[{0}] Time[{1:.1f}] count[{2}]", m_id, ms, count);
        });
    }

    ~ClientSession()
    {

    }

    void WaitForConnect(CameraControlAsyncSevice* service, ServerCompletionQueue* server_cq)
    {
        service->RequestVideoStream(&m_ctx, &m_request, &m_writer, &m_cq, server_cq, this);
    }

    void SendVideoStream(const GPacket& packet)
    {
        if (CONNECT == m_status)
        {
            m_locker_writer.wait();
            m_writer.Write(packet, Tag(WRITE));
        }
    }

    int Activate()
    {
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
        if (CONNECT == m_status)
        {
            m_writer.Finish(m_grpc_status, Tag(FINISH));
        }
        m_cq.Shutdown();

        if (m_thread.joinable())
            m_thread.join();

        return 0;
    }

    void ProcWork()
    {
        while (true)
        {
            void* got_tag = nullptr;
            bool  ok = false;

            if (!m_cq.Next(&got_tag, &ok)) 
            {
                LOGI << "Client stream closed. Quitting";
                break;
            }

            if (ok)
            {
                switch (Detag(got_tag))
                {
                case WRITE:
                    m_locker_writer.wakeup();
                    m_fps_checker.DoCheckFPS();
                    break;
                case FINISH:
                    LOGI << "Clinet FINISH...";
                    break;
                default:
                    LOGI << "Clinet out of range...";
                    break;
                }
            }
            else
            {
                LOGI << "Clinet cq not ok...: " << Detag(got_tag);
            }
        }
    }
};

//////////////////////////////////////////////////////////////////////////
// class Server_grpc

Server_grpc::Server_grpc()
{

}

Server_grpc::~Server_grpc()
{

}

int Server_grpc::Initialize()
{
    m_url = fmt::format("{0}:{1}", GetInfo().ip_address, GetInfo().port_number);
    LOGI << "grpc url : " << m_url;

    m_async_service.reset(new CameraControlAsyncSevice);

    ServerBuilder builder;
    builder.AddListeningPort(m_url, grpc::InsecureServerCredentials());
    builder.RegisterService(m_async_service.get());
    m_server_cq = builder.AddCompletionQueue();
    m_server = builder.BuildAndStart();
    if (nullptr == m_server)
        return 1;

    for (int ii = 0; ii < 4; ii++)
    {
        std::unique_ptr<ClientSession> client(new ClientSession((int)m_clients.size()));
        client->WaitForConnect(m_async_service.get(), m_server_cq.get());
        m_clients.push_back(std::move(client));
    }

    return 0;
}

int Server_grpc::Finalize()
{
    return 0;
}

int Server_grpc::Activate()
{
    LOGI << "Server Activate";

    m_thread = std::thread([this]() 
    {
        ProcClientConnection();
    });

    return 0;
}

int Server_grpc::Deactivate()
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

uint8_t* Server_grpc::AllocateVideoStreamBuffer(int size)
{
    if (m_video_stream_buffer.size() == size)
        return m_video_stream_buffer.data();

    m_video_stream_buffer.resize(size);
    std::fill(m_video_stream_buffer.begin(), m_video_stream_buffer.end(), 0);

    return m_video_stream_buffer.data();
}

int Server_grpc::PushVideoStream(ServerVideoStreamData& data)
{
    if (m_clients.empty())
        return 1;

    int buffer_len = data.buffer_len + GRPC_BUFFER_PADDING_SIZE;
    uint8_t* buffer = AllocateVideoStreamBuffer(buffer_len);

    GPacket packet_video_stream;
    CPacketWriter pk_writer(buffer, buffer_len);
    pk_writer.Start("VIDEO_STREAM_1");
    pk_writer.Push("DATA", data.buffer, data.buffer_len);
    m_video_stream_len = pk_writer.End();
    
    packet_video_stream.set_name("VIDEO_STREAM_1");
    packet_video_stream.set_data(m_video_stream_buffer.data(), m_video_stream_len);

    for (std::unique_ptr<ClientSession>& client : m_clients)
        client->SendVideoStream(packet_video_stream);

    return 0;
}

void Server_grpc::ProcClientConnection()
{
    while (true)
    {
        void* got_tag = nullptr;
        bool  ok = false;

        if (!m_server_cq->Next(&got_tag, &ok))
        {
            LOGI << "Client stream closed. Quitting";
            break;
        }

        if (ok)
        {
            LOGI << "Connect client...";

            ClientSession* clinet = static_cast<ClientSession*>(got_tag);            
            clinet->Activate();
        }
        else
        {
            LOGI << "Server cq not ok... : " << got_tag;
        }
    }
}

