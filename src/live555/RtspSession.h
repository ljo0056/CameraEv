#ifndef RTSPSESSION_H
#define RTSPSESSION_H

#include "InnerThread.h"

#include "Decoder.h"

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>

#include <string>

extern "C" {
#include <libavformat/avformat.h>
}

//#define _DEBUG_RTSP

class RtspSession : public InnerThread {
private:
#ifdef _DEBUG_RTSP
   static const int RTSP_CLIENT_VERBOSITY_LEVEL = 1;
#else
   static const int RTSP_CLIENT_VERBOSITY_LEVEL = 0;
#endif // _DEBUG_RTSP

public:
   struct Info {
      size_t camera_id = 0;
      std::string name;
      std::string url;
      std::string auth_id;
      std::string auth_pw;
      uint port    = 554;
      bool use_tcp = true;
   };

public:
   RtspSession( RingBuffer* ring_buffer );
   virtual ~RtspSession() override;

public:
   size_t id() const;
   const Info& info();
   void set_info( RtspSession::Info& info );
   void start( RtspSession::Info& info );
   bool start();
   void stop();
   void reconnect_rtsp();

   void stop_multi();
   void join_multi();

   RingBuffer* video_buffer();

protected:
   void ThreadLoop() override;
   bool openURL( UsageEnvironment& env, Info info );

   RTSPClient* m_rtspClient      = nullptr;
   char m_eventLoopWatchVariable = 0;

   Info m_info;
   bool m_thread_running      = false;
   RingBuffer* m_video_buffer = nullptr;
};

#endif // RTSPSESSION_H
