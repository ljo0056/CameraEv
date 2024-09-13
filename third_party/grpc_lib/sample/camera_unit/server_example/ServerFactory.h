#pragma once

// std c++
#include <functional>
#include <map>

#include "ServerDefine.h"

//////////////////////////////////////////////////////////////////////////
// class ServerImpl
//////////////////////////////////////////////////////////////////////////

class ServerImpl
{
private:
    ServerInfo    m_info;

public:
    ServerImpl();
    virtual ~ServerImpl();

    void SetInfo(const ServerInfo& param);
    const ServerInfo& GetInfo();

    virtual int Initialize();
    virtual int Finalize();

    virtual int Activate();
    virtual int Deactivate();

    virtual int PushVideoStream(ServerVideoStreamData& data);

    static std::shared_ptr<ServerImpl> Create()
    {
        return std::make_shared<ServerImpl>();
    }
};

//////////////////////////////////////////////////////////////////////////
// class DecoderFactory
//////////////////////////////////////////////////////////////////////////

class ServerFactory
{
private:
    typedef std::function<std::shared_ptr<ServerImpl>(void)> CreateCallback;
    static std::map<int, CreateCallback>   m_mapSessions;

public:

    static void Initialize();

    static void Register(int type, CreateCallback callback);
    static void Unregister(int type);

    static std::shared_ptr<ServerImpl> CreateFactory(int type);
};

