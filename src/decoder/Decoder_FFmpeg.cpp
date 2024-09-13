#include "stdafx.h"
#include "Decoder_FFmpeg.h"
#include "Converter_FFmpeg.h"
#include "FFmpeg.h"

#pragma warning(push)
#ifdef __SUPPORT_FFMPEG_VER_3
#pragma warning(disable:4996)     // error C4996 의 deprecated 선언으로 인해 컴파일이 안되는 문제로 추가 (FFMPEG Ver3, Ver4 의 차이)
#endif
#pragma warning(disable:4819)     // libavutil/rational.h 의 코드 페이지(949) warning C4819 off (Line 49)


//////////////////////////////////////////////////////////////////////////
// class DecoderImpl_FFmpeg
//////////////////////////////////////////////////////////////////////////

DecoderImpl_FFmpeg::DecoderImpl_FFmpeg()
    : DecoderImpl()
    , m_convert(std::make_unique<Converter_FFmpeg>())
{
    FFmpeg::Register();
}

DecoderImpl_FFmpeg::~DecoderImpl_FFmpeg()
{
    Deactivate();
}

int DecoderImpl_FFmpeg::Activate()
{
    m_avframe = av_frame_alloc();
    if (nullptr == m_avframe)
    {
        LOGE << "av_frame_alloc";
        return 1;
    }

    AVCodecID avcodec_id = FFmpeg::GetAVCodecID(GetInfo().vcodec_id);
    if (avcodec_id == AV_CODEC_ID_NONE)
    {
        LOGE << "codec type [" << GetInfo().vcodec_id << "]";
        return 2;
    }

    m_avcodec = avcodec_find_decoder(avcodec_id);
    if (nullptr == m_avcodec)
    {
        LOGE << "avcodec_find_decoder";
        return 3;
    }
    
    m_avcodec_context = avcodec_alloc_context3(m_avcodec);
    if (m_avcodec->capabilities & AV_CODEC_CAP_TRUNCATED)
        m_avcodec_context->flags |= AV_CODEC_CAP_TRUNCATED; // we do not send complete frames

    int ret = avcodec_open2(m_avcodec_context, m_avcodec, nullptr);
    if (ret < 0)
    {
        LOGE << "avcodec_open2: " << FFmpeg::ErrorCode(ret);
        return 4;
    }

    m_source_avpacket = av_packet_alloc();
    av_init_packet(m_source_avpacket);
    m_source_avpacket->flags |= AV_PKT_FLAG_KEY;
    m_source_avpacket->pts = 0;
    m_source_avpacket->dts = 0;

    return 0;
}

int DecoderImpl_FFmpeg::Deactivate()
{
    if (m_avframe)
    {
        av_frame_free(&m_avframe);
        m_avframe = nullptr;
    }

    if (m_avcodec_context) {
        avcodec_free_context(&m_avcodec_context);
        m_avcodec_context = nullptr;
    }

    if (m_source_avpacket)
    {
        av_packet_free(&m_source_avpacket);
        m_source_avpacket = nullptr;
    }

    return 0;
}

int DecoderImpl_FFmpeg::Decode(const VideoSource& src, DecoderVideoDest& dst)
{
    m_source_avpacket->data = src.frame.data;
    m_source_avpacket->size = src.frame.data_size;

#ifdef __SUPPORT_FFMPEG_VER_3
    int got_picture = 0;
    int ret = avcodec_decode_video2(m_avcodec_context, m_avframe, &got_picture, m_source_avpacket);
    if (ret < 0)
    {
        if (AVERROR_INVALIDDATA == ret) {
            // -1094995529 means "Invalid data found when processing input". We should wait for receive key frame;
            LOGW << "Warning decoding frame. " << FFmpeg::ErrorCode(ret);
            return DECODE_NEED_MORE_FRAME;
        }
        else {
            LOGE << "Error decoding frame. " << FFmpeg::ErrorCode(ret);
            return DECODE_FAILED;
        }
    }

    if (got_picture <= 0)
        return DECODE_FAILED;
#else
    int ret = avcodec_send_packet(m_avcodec_context, m_source_avpacket);
    if (ret != 0)
    {
        if (AVERROR_INVALIDDATA == ret) {
            // -1094995529 means "Invalid data found when processing input". We should wait for receive key frame;
            LOGW << "Warning decoding frame. " << FFmpeg::ErrorCode(ret);
            return DECODE_NEED_MORE_FRAME;
        }

        LOGE << "Error decoding frame. " << FFmpeg::ErrorCode(ret);
        return DECODE_FAILED;
    }
    ret = avcodec_receive_frame(m_avcodec_context, m_avframe);
    if (ret != 0)
    {
        LOGE << "Error receive frame " << FFmpeg::ErrorCode(ret);
        return DECODE_FAILED;
    }
#endif
    if (m_avcodec_context->width <= 0 || m_avcodec_context->width <= 0)
        return DECODE_NEED_MORE_FRAME;

    FrameInfo& frame_info = GetFrameInfoRef(FT_ORIGINAL);
    frame_info.width  = m_avcodec_context->width;
    frame_info.height = m_avcodec_context->height;

    ret = DECODE_SUCCESS;
    for (int ii = 0 ; ii < FT_NUM ; ii++)
    {
        if (nullptr == dst.frame[ii].data || dst.frame[ii].data_size < 128)
            continue;

        FrameInfo& frame_info = GetFrameInfoRef((FRAME_TYPE)ii);
        if (false == frame_info.decode)
            continue;
        if (VPIX_FMT_NONE == frame_info.pixel_format)
            continue;

        Converter_FFmpeg::ConvertParam param;
        // src
        param.src_pixel_format  = m_avcodec_context->pix_fmt;
        param.src_width         = m_avcodec_context->width;
        param.src_height        = m_avcodec_context->height;
        // dst
        param.dst_buffer        = dst.frame[ii].data;
        param.dst_pixel_format  = FFmpeg::GetToAVPixelFormat(frame_info.pixel_format);
        param.dst_width         = frame_info.width;
        param.dst_height        = frame_info.height;

        int buffer_size = FFmpeg::GetImageBufferSize(param.dst_pixel_format, param.dst_width, param.dst_height, 1);
        if (dst.frame[ii].data_size < buffer_size) // 버퍼 사이즈가 작을 시 체크
            continue;

        if (m_convert->Convert(m_avframe, param))
            ret = DECODE_CONVERT_FAILED;
    }

    return ret;
}

#pragma warning(pop)