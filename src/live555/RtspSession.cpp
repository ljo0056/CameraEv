#include "RtspSession.h"

#include "rtsp_client.h"
#include "rtsp_callback.h"

#include "Common.h"
#include "Logger_yhpark.hpp"

#include <liveMedia/DigestAuthentication.hh>

std::ostream& operator<<( std::ostream& ss, const RtspSession& obj )
{
   ss << "ch[" << obj.id() << "] ";
   return ss;
}

RtspSession::RtspSession( RingBuffer* input_buffer )
    : m_video_buffer( input_buffer )
{
}

RtspSession::~RtspSession()
{
   stop();
}

size_t RtspSession::id() const
{
   return m_info.camera_id;
}

void RtspSession::set_info( RtspSession::Info& info )
{
   m_info = info;
}

void RtspSession::start( RtspSession::Info& info )
{
   set_info( info );
   start();
}

bool RtspSession::start()
{
   if( m_thread_running ) {
      return false;
   }

   InnerThread::SaveThreadName( m_info.name.c_str() );
   InnerThread::StartThread( this );

   //TODO(yhpark): exception for failed to connect.
   return true;
}

void RtspSession::stop()
{
   m_eventLoopWatchVariable = 1;
   m_thread_running         = false;
   InnerThread::JoinThread();
}

void RtspSession::stop_multi()
{
   m_eventLoopWatchVariable = 1;
   m_thread_running         = false;
}

void RtspSession::join_multi()
{
   InnerThread::JoinThread();
}

void RtspSession::reconnect_rtsp()
{
   m_eventLoopWatchVariable = 1;
}

void RtspSession::ThreadLoop()
{
   m_thread_running = true;

   TaskScheduler* scheduler = BasicTaskScheduler::createNew();
   UsageEnvironment* env    = BasicUsageEnvironment::createNew( *scheduler );

   while( m_thread_running ) {

      if( openURL( *env, m_info ) ) {

         LOGI << *this << "Rtsp doEventLoop starting...";
         m_eventLoopWatchVariable = 0;
         env->taskScheduler().doEventLoop( &m_eventLoopWatchVariable );

         LOGD << *this << "Rtsp Shutdowning..";
         shutdownStream( m_rtspClient );
      }

      if( m_thread_running ) {
         LOGW << *this << "Rtsp re-connecting... 3 sec interval.";
         Sleep_ms( 3000 );
      }
   }

   env->reclaim();
   env = nullptr;
   delete scheduler;
   scheduler = nullptr;

   LOGI << *this << "Rtsp Shutdowned, and Finished.";
}

bool RtspSession::openURL( UsageEnvironment& env, Info info )
{
   m_rtspClient = CustomRTSPClient::createNew(
       this,
       env,
       info.url.c_str(),
       RTSP_CLIENT_VERBOSITY_LEVEL,
       info.name.c_str() );

   if( m_rtspClient == nullptr ) {
      LOGE << *this << "Failed to create a RTSP client for URL \"" << m_info.url << "\": " << env.getResultMsg();
      return false;
   }

   // Autentification.
   Authenticator* authenticator = nullptr;
   if( m_info.auth_id.empty() == false && m_info.auth_pw.empty() == false ) {
      authenticator = new Authenticator( m_info.auth_id.c_str(), m_info.auth_pw.c_str() );
   }
   uint ret = m_rtspClient->sendDescribeCommand( continueAfterDESCRIBE, authenticator );
   if( authenticator ) {
      delete authenticator;
      authenticator = nullptr;
   }

   if( ret == 0 ) {
      LOGE << *this << "Open URL Failed( sendDescribeCommand )";
      return false;
   }

   return true;
}

const RtspSession::Info& RtspSession::info()
{
   return m_info;
}

RingBuffer* RtspSession::video_buffer()
{
   return m_video_buffer;
}
