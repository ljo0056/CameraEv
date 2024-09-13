#include "stdafx.h"
#include "rtsp_H264Sink.h"

#include <liveMedia/include/BitVector.hh>

#include "session/Session_RTSP.h"
#include "decoder/DecoderDefine.h"

H264Sink* H264Sink::createNew(UsageEnvironment& env, Session_RTSP* session, MediaSubsession& subsession, char const* streamId)
{
    return new H264Sink(env, session, subsession, streamId);
}

H264Sink::H264Sink(UsageEnvironment& env, Session_RTSP* session, MediaSubsession& subsession, char const* streamId)
    : MediaSink(env)
    , m_media_subsession(subsession)
    , m_session(session)
{
    LOGD << "Codec Info : " << subsession.codecName();
    if (std::string("H264") == subsession.codecName()) {
    }
    else {
        LOGE << "H264 codec not found!";
    }

    m_stream_id = streamId;

    if (setSpropRecords()) {
        LOGD << "setSpropRecords done";

        if (m_session && m_session->GetStreamCallback().callback_video_info_changed)
        {
            SessionVideoInfo video_info;
            video_info.width  = m_media_subsession.videoWidth();
            video_info.height = m_media_subsession.videoHeight();
            video_info.fps    = (float)m_media_subsession.videoFPS();
            m_session->GetStreamCallback().callback_video_info_changed(video_info);
        }
    }
    else {
        LOGE << "failed to setSpropRecords.";
    }

    OutPacketBuffer::increaseMaxSizeTo(1024 * 10); // bytes
}

H264Sink::~H264Sink()
{
    LOGI;
}

void H264Sink::afterGettingFrame(void* clientData, unsigned recv_packet_size, unsigned numTruncatedBytes, ::timeval presentationTime, unsigned durationInMicroseconds)
{
    reinterpret_cast<H264Sink*>(clientData)->afterGettingFrame(recv_packet_size, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

bool H264Sink::isKeyFrame(byte* paket)
{
    // RFC 6184 에 나와있는 "Network Abstraction Layer Unit Types" 내용을 인용함.
    //   +---------------+
    //   |0|1|2|3|4|5|6|7|
    //   +-+-+-+-+-+-+-+-+
    //   |F|NRI|  Type   |
    //   +---------------+
    
    // F    (1 bit): forbidden_zero_bit.  The H.264 specification declares a value of 1 as a syntax violation.
    // NRI  (2 bit): nal_ref_idc.  A value of 00 indicates that the content of the NAL unit is not used to reconstruct reference pictures
    //               for inter picture prediction.  Such NAL units can be discarded without risking the integrity of the reference pictures.
    //               Values greater than 00 indicate that the decoding of the NAL unit is required to maintain the integrity of the reference pictures.
    // Type (3 bit): nal_unit_type.   This component specifies the NAL unit payload type
    
    // nal_unit_type
    // ITU-T H.264 Advanced video coding for generic audiovisual services 문서 참조함.
    
    //  0: Unspecified
    //  1: Coded slice of a non-IDR picture
    //  2: Coded slice data partition A
    //  3: Coded slice data partition B
    //  4: Coded slice data partition C
    //  5: Coded slice of an IDR picture               
    //  6: Supplemental enhancement information (SEI)
    //  7: Sequence parameter set
    //  8: Picture parameter set
    //  9: Access unit delimiter
    // 10: End of sequence
    // 11: End of stream
    // 12: Filler data
    // 13: Sequence parameter set extension
    // 14: Prefix NAL unit
    // 15: Subset sequence parameter set
    // 16~18: Reserved
    // 19: Coded slice of an auxiliary coded picture without partitioning
    // 20: Coded slice extension
    // 21~23: Reserved
    // 24~31: Unspecified

#if 0
    // 참조
    // https://stackoverflow.com/questions/1957427/detect-mpeg4-h264-i-frame-idr-in-rtp-stream
    int RTPHeaderBytes = 0;

    int fragment_type = paket[RTPHeaderBytes + 0] & 0x1F;
    int nal_type      = paket[RTPHeaderBytes + 1] & 0x1F;
    int start_bit     = paket[RTPHeaderBytes + 1] & 0x80;

    if (((fragment_type == 28 || fragment_type == 29) && nal_type == 5 && start_bit == 128) || fragment_type == 5)
    {
        return true;
    }

    return false;
#else
    enum H264_NAL_UNIT_TYPE
    {
        NAL_UNIT_TYPE_NON_IDR_SLICE = 1, // P-Frame
        NAL_UNIT_TYPE_DP_A_SLICE    = 2,
        NAL_UNIT_TYPE_DP_B_SLICE    = 3,
        NAL_UNIT_TYPE_DP_C_SLICE    = 4,
        NAL_UNIT_TYPE_IDR_SLICE     = 5, // I-Frame
        NAL_UNIT_TYPE_SEI           = 6,
        NAL_UNIT_TYPE_SEQ_PARAM     = 7,
        NAL_UNIT_TYPE_PIC_PARAM     = 8,
        NAL_UNIT_TYPE_ACCESS_UNIT   = 9,
        NAL_UNIT_TYPE_END_OF_SEQ    = 10,
        NAL_UNIT_TYPE_END_OF_STREAM = 11,
        NAL_UNIT_TYPE_FILLER_DATA   = 12,
        NAL_UNIT_TYPE_SEQ_EXTENSION = 13,
    };

    BYTE nNALHeader         = m_rtp_packet_buffer[0];
    BYTE forbidden_zero_bit = (nNALHeader & 0x80) >> 7;
    BYTE nal_ref_idc        = (nNALHeader & 0x60) >> 5;
    BYTE nal_unit_type      = (nNALHeader & 0x1F);

    bool key_frame = false;
    switch (nal_unit_type) {
    case NAL_UNIT_TYPE_ACCESS_UNIT:
        break;
    case NAL_UNIT_TYPE_SEI:
        break;
    case NAL_UNIT_TYPE_SEQ_PARAM:
        break;
    case NAL_UNIT_TYPE_PIC_PARAM:
        break;
    case NAL_UNIT_TYPE_IDR_SLICE:
        key_frame = true;
        break;
    default:
        break;
    }

    return key_frame;
#endif
}

void H264Sink::afterGettingFrame(unsigned recv_packet_size, unsigned numTruncatedBytes, ::timeval presentationTime, unsigned durationInMicroseconds)
{
    bool key_frame = isKeyFrame(m_rtp_packet_buffer.data());

    RTPSource* rtpsrc = m_media_subsession.rtpSource();
    if (rtpsrc == nullptr) {
        LOGE << "rtpsrc is nullptr.";
        continuePlaying();
        return;
    }

    bool is_packet_finished           = rtpsrc->curPacketMarkerBit();
    bool hasBeenSynchronizedUsingRTCP = m_media_subsession.rtpSource()->hasBeenSynchronizedUsingRTCP();
    double play_time_current          = m_media_subsession.getNormalPlayTime(presentationTime);

#ifdef _DEBUG_RTSP_PACKET
    std::stringstream debug_msg;

    RTPReceptionStatsDB::Iterator iter(rtpsrc->receptionStatsDB());
    RTPReceptionStats* stats = iter.next(true);

    if (numTruncatedBytes > 0) {
        debug_msg << " numTruncatedBytes[" << numTruncatedBytes << "]";
    }
    if (durationInMicroseconds > 0) {
        debug_msg << "durationInMicroseconds[" << durationInMicroseconds << "]";
    }
    if (rtpsrc != nullptr) {
        debug_msg << "marker[" << is_packet_finished << "]";
    }
    if (stats != nullptr) {
        debug_msg << " SSRC[" << stats->SSRC() << "]";
    }
    debug_msg << " recv[" << recv_packet_size << "]";
    if (presentationTime.tv_sec != 0 || presentationTime.tv_usec != 0) {
        struct tm st_tm;
        localtime_r(&presentationTime.tv_sec, &st_tm);
        std::array<char, 128> buf;
        size_t len = strftime(buf.data(), buf.size(), "%Y/%m/%d %r %T.", &st_tm);
        sprintf(buf.data() + len, "%u", uint(uint(presentationTime.tv_usec) / 1000));
        debug_msg << " ts: " << buf.data();
        debug_msg << " " << (hasBeenSynchronizedUsingRTCP ? "Sync" : "NoSync") << "NPT:" << play_time_current;
    }
    LOGD << debug_msg.str();
    LOGD << "recv_packet_size:" << recv_packet_size;
    LOGD << " presentationTime:" << presentationTime.tv_sec << " " << presentationTime.tv_usec;
    LOGD_IF(numTruncatedBytes != 0) << "numTruncatedBytes:" << numTruncatedBytes;
    LOGD_IF(durationInMicroseconds > 0) << " durationInMicroseconds:" << durationInMicroseconds;
#endif // _DEBUG_RTSP_PACKET

    enum ParseMode {
        ParseMode_Normal,
        ParseMode_ForStupidRtspSever,
    };
    ParseMode mode = ParseMode_Normal;

    if (mode == ParseMode_Normal) {

        if (is_packet_finished) {
            appendH264HeaderPacket();
        }

        // body Packet.
        appendH264BodyPacket(recv_packet_size);

        if (is_packet_finished) {
            // packet finished.
            if (m_session && m_session->GetStreamCallback().callback_video_frame_received)
            {
                SessionVideoFrame video_frame;
                video_frame.key_frame  = key_frame;
                video_frame.codec_id   = VCODEC_ID_H264;
                video_frame.s_buffer   = m_received_buffer.data();
                video_frame.s_buf_size = (int)m_received_buffer.len();
                video_frame.timestamp  = presentationTime;
                m_session->GetStreamCallback().callback_video_frame_received(video_frame);
            }

            m_received_buffer.clear();
        }
    }
    else if (mode == ParseMode_ForStupidRtspSever) {
        bool is_new_packet = m_play_time_previous != play_time_current;
        if (is_new_packet && m_received_buffer.len() > 0) {
            is_packet_finished = true;
        }

        if (is_packet_finished) {
            appendH264HeaderPacket();
        }

        // body Packet.
        appendH264BodyPacket(recv_packet_size);

        if (is_packet_finished) {

            // packet finished.
            if (m_session && m_session->GetStreamCallback().callback_video_frame_received)
            {
                SessionVideoFrame video_frame;
                video_frame.key_frame  = key_frame;
                video_frame.codec_id   = VCODEC_ID_H264;
                video_frame.s_buffer   = m_received_buffer.data();
                video_frame.s_buf_size = (int)m_received_buffer.len();
                video_frame.timestamp  = presentationTime;
                m_session->GetStreamCallback().callback_video_frame_received(video_frame);
            }

            m_received_buffer.clear();
        }

        m_timeval_previous = presentationTime;
        m_play_time_previous = play_time_current;
    }

    // Then continue, to request the next frame of data:
    continuePlaying();
}

Boolean H264Sink::continuePlaying()
{
    if (fSource == nullptr) {
        LOGF << "sanity check (should not happen)";
        return false;
    }

    // Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
    fSource->getNextFrame(m_rtp_packet_buffer.data(), g_rtp_packet_buffer_size, afterGettingFrame, this, onSourceClosure, this);

    return true;
}

bool H264Sink::setSpropRecords()
{
    const char* sprop = m_media_subsession.fmtp_spropparametersets();
    if (sprop != nullptr) {
        unsigned numSPropRecords  = 0;
        SPropRecord* sPropRecords = parseSPropParameterSets(sprop, numSPropRecords);

        if (sPropRecords)
        {
            m_header_buffer.clear();

            for (unsigned i = 0; i < numSPropRecords; ++i) 
            {
                if (sPropRecords[i].sPropLength == 0) {
                    LOGW << "sPropRecords[i].sPropLength == 0";
                    continue;
                }

                u_int8_t nal_unit_type = (sPropRecords[i].sPropBytes[0]) & 0x1F;
                if (nal_unit_type == 7) {
                    // 7 = SPS.
                    SPropRecord* sPropRecords_sps = (sPropRecords + i);
                    m_header_buffer.append(g_H264StartCode.data(), g_H264StartCode.size());
                    m_header_buffer.append(sPropRecords_sps->sPropBytes, sPropRecords_sps->sPropLength);
                }
                else if (nal_unit_type == 8) {
                    // 8 = PPS.
                    SPropRecord* sPropRecords_pps = (sPropRecords + i);
                    m_header_buffer.append(g_H264StartCode.data(), g_H264StartCode.size());
                    m_header_buffer.append(sPropRecords_pps->sPropBytes, sPropRecords_pps->sPropLength);
                }
            }

            delete[] sPropRecords;
        }
    }

    return m_header_buffer.len() > 0 ? true : false;
}

void H264Sink::appendPacket(const uint8_t* data, size_t len)
{
    m_received_buffer.append(data, len);
}

void H264Sink::appendH264HeaderPacket()
{
    appendPacket(m_header_buffer.data(), m_header_buffer.len());
}

void H264Sink::appendH264BodyPacket(size_t recv_packet_size)
{
    appendPacket(g_H264StartCode.data(), g_H264StartCode.size());
    appendPacket(m_rtp_packet_buffer.data(), recv_packet_size);
}
