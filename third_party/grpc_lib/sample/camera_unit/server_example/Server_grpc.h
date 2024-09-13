#pragma once

#include "ServerFactory.h"

#include <thread>
#include <string>
#include <vector>
#include <mutex>

namespace grpc
{
    class Server;
    class ServerCompletionQueue;
}


class ClientSession;
class CameraControlSevice;
class CameraControlAsyncSevice;


//////////////////////////////////////////////////////////////////////////
// class Server_grpc

class Server_grpc : public ServerImpl
{
private:
    std::thread                                     m_thread;

    std::string                                     m_url;
    std::unique_ptr<CameraControlAsyncSevice>       m_async_service;
    std::unique_ptr<grpc::Server>                   m_server;
    std::unique_ptr<grpc::ServerCompletionQueue>    m_server_cq;

    std::recursive_mutex                            m_mutex_clients;
    std::vector<std::unique_ptr<ClientSession>>     m_clients;

    std::vector<uint8_t>                            m_video_stream_buffer;
    int                                             m_video_stream_len = 0;

private:
    void ProcClientConnection();
    uint8_t* AllocateVideoStreamBuffer(int size);

public:
    Server_grpc();
    virtual ~Server_grpc();

    virtual int Initialize() override;
    virtual int Finalize() override;

    virtual int Activate() override;
    virtual int Deactivate() override;

    virtual int PushVideoStream(ServerVideoStreamData& data) override;

    static std::shared_ptr<Server_grpc> Create()
    {
        return std::make_shared<Server_grpc>();
    }
};

