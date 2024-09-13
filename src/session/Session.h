#pragma once

#include "SessionDefine.h"
#include <memory>

//////////////////////////////////////////////////////////////////////////
// class Session
//////////////////////////////////////////////////////////////////////////

class Session
{
private:
    SessionInfo         m_info;

    StreamCallback      m_stream_callback;
    ControlCallback     m_control_callback;
    PTZCallback         m_ptz_callback;

public:
    Session();
    virtual ~Session();

    int SetSessionInfo(const SessionInfo& param);
    const SessionInfo& GetSessionInfo() const;

    void SetStreamCallback(const StreamCallback& callback);
    const StreamCallback& GetStreamCallback() const;

    virtual int Initialize();
    virtual int Finalize();

    virtual int Activate();
    virtual int Deactivate();

    static std::shared_ptr<Session> Create()
    {
        return std::make_shared<Session>();
    }
};
