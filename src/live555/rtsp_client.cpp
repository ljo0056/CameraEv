#include "stdafx.h"
#include "rtsp_client.h"

// Implementation of "ourRTSPClient":

CustomRTSPClient* CustomRTSPClient::createNew(
    Session_RTSP* session,
    UsageEnvironment& env,
    char const* rtspURL,
    int verbosityLevel,
    char const* applicationName,
    portNumBits tunnelOverHTTPPortNum)
{
    return new CustomRTSPClient(session, env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

CustomRTSPClient::CustomRTSPClient(
    Session_RTSP* session,
    UsageEnvironment& env,
    char const* rtspURL,
    int verbosityLevel,
    char const* applicationName,
    portNumBits tunnelOverHTTPPortNum)
    : RTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1)
    , m_session(session)
{
}

CustomRTSPClient::~CustomRTSPClient()
{
}

StreamClientState& CustomRTSPClient::getStreamClientState()
{
    return m_streamClientState;
}

Session_RTSP* CustomRTSPClient::getSession()
{
    return m_session;
}