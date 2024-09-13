#pragma once

#include <memory>
#include "ServerDefine.h"

//////////////////////////////////////////////////////////////////////////
// class Server
//////////////////////////////////////////////////////////////////////////
/// class server description
/// - 외부에서 Camera 의 제어 및 영상의 수신을 담당하는 통신 모듈 입니다.
/// - 다양한 외부 라이브러리를 이용하여 만들 수 있도록 Factory patten 으로 되어 있습니다.
/// - 다양한 외부 라이브러리로는 (grpc, shared memory, redis, memcached 등을 사용 할 수 있습니다.
/// - 2019.05.28 grpc 를 이용한 외부 모듈 구현

class ServerImpl;

class Server
{
private:
    std::shared_ptr<ServerImpl>    m_impl;

public:
    Server();
    virtual ~Server();

    int Initialize(const ServerInfo& param);
    int Finalize();

    int Activate();
    int Deactivate();

    int PushVideoStream(ServerVideoStreamData& data);
};
