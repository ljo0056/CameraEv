#include "stdafx.h"
#include "Server.h"
#include "ServerFactory.h"


Server::Server()
{
    ServerFactory::Initialize();
}

Server::~Server()
{
    
}

int Server::Initialize(const ServerInfo& param)
{
    m_impl = ServerFactory::CreateFactory(param.type);
    if (nullptr == m_impl)
        return 1;

    m_impl->SetInfo(param);
    if (m_impl->Initialize())
        return 2;

    return 0;
}

int Server::Finalize()
{
    if (m_impl)
        m_impl->Finalize();

    return 0;
}

int Server::Activate()
{
    if (m_impl)
    {
        if (m_impl->Activate())
            return 1;
    }

    return 0;
}

int Server::Deactivate()
{
    if (m_impl)
        m_impl->Deactivate();

    return 0;
}

int Server::PushVideoStream(ServerVideoStreamData& data)
{
    if (nullptr == m_impl)
        return 1;

    return m_impl->PushVideoStream(data);
}