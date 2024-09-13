#include "stdafx.h"
#include "SessionFactory.h"

#include "Session_RTSP.h"

//////////////////////////////////////////////////////////////////////////
// class SessionFactory
//////////////////////////////////////////////////////////////////////////

std::map<int, SessionFactory::CreateCallback> SessionFactory::m_mapSessions;

void SessionFactory::Register(int type, CreateCallback callback)
{
    m_mapSessions[type] = callback;
}

void SessionFactory::Unregister(int type)
{
    m_mapSessions.erase(type);
}

std::shared_ptr<Session> SessionFactory::CreateFactory(int type)
{
    auto it = m_mapSessions.find(type);
    if (it != m_mapSessions.end())
    {
        return (it->second)();
    }

    return std::shared_ptr<Session>();
}

void SessionFactory::Initialize()
{
    Register(SESSION_TYPE_RTSP, Session_RTSP::Create);
}
