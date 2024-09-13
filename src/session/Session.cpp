#include "stdafx.h"
#include "Session.h"

Session::Session()
{
    
}

Session::~Session()
{
    
}

int Session::SetSessionInfo(const SessionInfo& param)
{
    m_info = param;

    return 0;
}

const SessionInfo& Session::GetSessionInfo() const
{
    return m_info;
}

void Session::SetStreamCallback(const StreamCallback& callback)
{
    m_stream_callback = callback;
}

const StreamCallback& Session::GetStreamCallback() const
{
    return m_stream_callback;
}

int Session::Initialize()
{

    return 0;
}

int Session::Finalize()
{
    return 0;
}

int Session::Activate()
{

    return 0;
}

int Session::Deactivate()
{
    return 0;
}
