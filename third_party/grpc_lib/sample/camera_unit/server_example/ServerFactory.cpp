#include "stdafx.h"
#include "ServerFactory.h"
#include "ServerDefine.h"
#include "Server_grpc.h"

//////////////////////////////////////////////////////////////////////////
// class ServerImpl
//////////////////////////////////////////////////////////////////////////

ServerImpl::ServerImpl()
{
    
}

ServerImpl::~ServerImpl()
{

}

void ServerImpl::SetInfo(const ServerInfo& param)
{
    m_info = param;
}

const ServerInfo& ServerImpl::GetInfo()
{
    return m_info;
}

int ServerImpl::Initialize()
{
    return 0;
}

int ServerImpl::Finalize()
{
    return 0;
}

int ServerImpl::Activate()
{
    return 0;
}

int ServerImpl::Deactivate()
{
    return 0;
}

int ServerImpl::PushVideoStream(ServerVideoStreamData& data)
{
    return 0;
}


//////////////////////////////////////////////////////////////////////////
// class ServerFactory
//////////////////////////////////////////////////////////////////////////

std::map<int, ServerFactory::CreateCallback> ServerFactory::m_mapSessions;

void ServerFactory::Register(int type, CreateCallback callback)
{
    m_mapSessions[type] = callback;
}

void ServerFactory::Unregister(int type)
{
    m_mapSessions.erase(type);
}

std::shared_ptr<ServerImpl> ServerFactory::CreateFactory(int type)
{
    auto it = m_mapSessions.find(type);
    if (it != m_mapSessions.end())
    {
        return (it->second)();
    }

    return std::shared_ptr<ServerImpl>();
}

void ServerFactory::Initialize()
{
    
}
