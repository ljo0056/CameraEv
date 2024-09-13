#pragma once

#include "Session.h"
#include "common/InnerThread.h"
#include "common/Locker.h"

class RTSPClient;
class UsageEnvironment;

//////////////////////////////////////////////////////////////////////////
// class SessionRTSP
//////////////////////////////////////////////////////////////////////////

class Session_RTSP : public Session, public InnerThread
{
private:
    bool        m_thread_running = false;

    char        m_eventLoopWatchVariable = 0;
    RTSPClient* m_rtspClient = nullptr;
    Locker      m_locker;

private:
    bool openURL(UsageEnvironment& env);

public:
    Session_RTSP();
    virtual ~Session_RTSP();

    void Reconnect();

    // class Session override function...
    virtual int Initialize() override;
    virtual int Finalize() override;

    virtual int Activate() override;
    virtual int Deactivate() override;

    // class InnerThread override function...
    virtual void ThreadLoop() override;

    static std::shared_ptr<Session> Create()
    {
        return std::make_shared<Session_RTSP>();
    }
};
