#ifndef CAMERA_RTSP_CLIENT_H
#define CAMERA_RTSP_CLIENT_H

#include <liveMedia/include/liveMedia.hh>
#include <BasicUsageEnvironment/include/BasicUsageEnvironment.hh>
//#include <UsageEnvironment/include/UsageEnvironment.hh>

#include "rtsp_StreamClientState.h"
#include "session/Session_RTSP.h"

// If you're streaming just a single stream (i.e., just from a single URL, once), then you can define and use just a single
// "StreamClientState" structure, as a global variable in your application.  However, because - in this demo application - we're
// showing how to play multiple streams, concurrently, we can't do that.  Instead, we have to have a separate "StreamClientState"
// structure for each "RTSPClient".  To do this, we subclass "RTSPClient", and add a "StreamClientState" field to the subclass:


class CustomRTSPClient : public RTSPClient {
public:
   static CustomRTSPClient* createNew(
       Session_RTSP* session,
       UsageEnvironment& env,
       char const* rtspURL,
       int verbosityLevel                = 0,
       char const* applicationName       = nullptr,
       portNumBits tunnelOverHTTPPortNum = 0 );

protected:
   CustomRTSPClient(
                     Session_RTSP* session,
                     UsageEnvironment& env,
                     char const* rtspURL,
                     int verbosityLevel,
                     char const* applicationName,
                     portNumBits tunnelOverHTTPPortNum );
   // called only by createNew();
   virtual ~CustomRTSPClient();

public:
   StreamClientState&   getStreamClientState();
   Session_RTSP*        getSession();

private:
   Session_RTSP*        m_session = nullptr;
   StreamClientState    m_streamClientState;
};

#endif // CAMERA_RTSP_CLIENT_H
