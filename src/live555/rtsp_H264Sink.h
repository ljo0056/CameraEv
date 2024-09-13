#ifndef H264_SINK_H
#define H264_SINK_H

#include <liveMedia/include/liveMedia.hh>
#include <BasicUsageEnvironment/include/BasicUsageEnvironment.hh>

#include <array>

//////////////////////////////////////////////////////////////////////////

template<class T, std::size_t N>
class array_ex : public std::array<T, N>
{
public:
    size_t m_len = 0;

    size_t append(const T* buffer, size_t len)
    {
        if (m_len + len > N)
            return 0;

        memcpy(this->data() + m_len, buffer, len);
        m_len += len;

        return m_len;
    }

    size_t len()
    {
        return m_len;
    }

    void clear()
    {
        m_len = 0;
    }
};

//////////////////////////////////////////////////////////////////////////

class Session_RTSP;

static const u_int g_rtp_packet_buffer_size = 425984;
static const std::array<uint8_t, 4> g_H264StartCode{ { 0x00, 0x00, 0x00, 0x01 } };

class H264Sink : public MediaSink {
public:
   static H264Sink* createNew( UsageEnvironment& env,
                               Session_RTSP* session,
                               MediaSubsession& subsession, // identifies the kind of data that's being received
                               char const* streamId = nullptr ); // identifies the stream itself (optional)

private:
   H264Sink( UsageEnvironment& env, Session_RTSP* session, MediaSubsession& subsession, char const* streamId ); // called only by "createNew()"
   virtual ~H264Sink() override;

   static void afterGettingFrame( void* clientData, unsigned frameSize, unsigned numTruncatedBytes, ::timeval presentationTime, unsigned durationInMicroseconds );
   void afterGettingFrame( unsigned frameSize, unsigned numTruncatedBytes, ::timeval presentationTime, unsigned durationInMicroseconds );
   Boolean continuePlaying() override;

   bool setSpropRecords();
   void appendPacket( const uint8_t* data, size_t len );
   void appendH264HeaderPacket();
   void appendH264BodyPacket( size_t recv_packet_size );
   bool isKeyFrame(byte* paket);

   MediaSubsession&     m_media_subsession;
   Session_RTSP*        m_session = nullptr;
   std::string          m_stream_id;

   timeval  m_timeval_previous      = {};
   double   m_play_time_previous    = -1;

   array_ex<u_int8_t, 1024>                             m_header_buffer;
   array_ex<u_int8_t, g_rtp_packet_buffer_size>         m_rtp_packet_buffer;
   array_ex<u_int8_t, g_rtp_packet_buffer_size + 32>    m_received_buffer;
};

#endif // H264_SINK_H
