#include "stdafx.h"
#include "Session_RTSP.h"

#include "live555/rtsp_client.h"
#include "live555/rtsp_callback.h"

//////////////////////////////////////////////////////////////////////////
// plog
//////////////////////////////////////////////////////////////////////////

namespace plog
{
    Record& operator<<(Record& record, const Session_RTSP& session)
    {
        record << "ch[" << session.GetSessionInfo().name << "] ";
        return record;
    }
}

//////////////////////////////////////////////////////////////////////////
// Session_RTSP
//////////////////////////////////////////////////////////////////////////

Session_RTSP::Session_RTSP()
    : Session()
{

}

Session_RTSP::~Session_RTSP()
{
    
}

int Session_RTSP::Initialize()
{
    m_eventLoopWatchVariable = 0;

    return 0;
}

int Session_RTSP::Finalize()
{
    return 0;
}

int Session_RTSP::Activate()
{
    if (m_thread_running)
        return 0;   // 이미 동작 중

    InnerThread::SaveThreadName(GetSessionInfo().name);
    InnerThread::StartThread();

    return 0;
}

int Session_RTSP::Deactivate()
{
    m_eventLoopWatchVariable = 1;
    m_thread_running = false;
    m_locker.WakeUp();

    InnerThread::JoinThread();

    return 0;
}

void Session_RTSP::Reconnect()
{
    m_eventLoopWatchVariable = 1;
}

void Session_RTSP::ThreadLoop()
{
    m_thread_running = true;

    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    UsageEnvironment* env    = BasicUsageEnvironment::createNew(*scheduler);

    while (m_thread_running)
    {
        if (openURL(*env))
        {
            //LOGI << *this << "Rtsp doEventLoop starting...";
            m_eventLoopWatchVariable = 0;
            env->taskScheduler().doEventLoop(&m_eventLoopWatchVariable);

            //LOGD << *this << "Rtsp Shutdowning..";
            shutdownStream(m_rtspClient);   // 이 함수 안에서 m_rtspClient 객체를 delete 해준다.
            m_rtspClient = nullptr;
        }

        if (m_thread_running)
        {
            //LOGW << *this << "Rtsp re-connecting...";
            //Sleep(1000 * 10);
            m_locker.Wait();
        }
    }

    env->reclaim();
    env = nullptr;
    delete scheduler;
    scheduler = nullptr;

    LOGI << "Rtsp Shutdowned, and Finished.";
}

bool Session_RTSP::openURL(UsageEnvironment& env)
{
    const SessionInfo& info = GetSessionInfo();

#ifdef _DEBUG_RTSP
    static const int RTSP_CLIENT_VERBOSITY_LEVEL = 1;
#else
    static const int RTSP_CLIENT_VERBOSITY_LEVEL = 0;
#endif // _DEBUG_RTSP

    m_rtspClient = CustomRTSPClient::createNew(
        this,
        env,
        info.url.c_str(),
        RTSP_CLIENT_VERBOSITY_LEVEL,
        info.name.c_str());

    if (m_rtspClient == nullptr) {
        LOGE << *this << "Failed to create a RTSP client for URL \"" << info.url << "\": " << env.getResultMsg();
        return false;
    }

    // Autentification.
    Authenticator* authenticator = nullptr;
    if (info.auth_id.empty() == false && info.auth_pw.empty() == false) {
        authenticator = new Authenticator(info.auth_id.c_str(), info.auth_pw.c_str());
    }

    unsigned ret = m_rtspClient->sendDescribeCommand(continueAfterDESCRIBE, authenticator);
    if (authenticator) {
        delete authenticator;
        authenticator = nullptr;
    }

    if (ret == 0) {
        LOGE << *this << "Open URL Failed( sendDescribeCommand )";
        return false;
    }

    return true;
}