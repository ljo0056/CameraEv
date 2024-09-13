#include "stdafx.h"
#include "Converter_FFmpeg.h"
#include "FFmpeg.h"

#pragma warning(push)
#ifdef __SUPPORT_FFMPEG_VER_3
#pragma warning(disable:4996)     // error C4996 의 deprecated 선언으로 인해 컴파일이 안되는 문제로 추가  (FFMPEG Ver3, Ver4 의 차이)
#endif
#pragma warning(disable:4819)     // libavutil/rational.h 의 코드 페이지(949) warning C4819 off

//////////////////////////////////////////////////////////////////////////
// class Converter_FFmpeg
//////////////////////////////////////////////////////////////////////////

Converter_FFmpeg::Converter_FFmpeg()
    : m_avframe_src(nullptr)
    , m_avframe_dst(nullptr)
    , m_sws_contexts(nullptr)
{
    FFmpeg::Register();
}

Converter_FFmpeg::~Converter_FFmpeg()
{
    Finalize();
}

bool Converter_FFmpeg::IsEqualParam(const ConvertParam& param)
{
    if (param.src_pixel_format != m_param.src_pixel_format)     return false;
    if (param.src_width        != m_param.src_width)            return false;
    if (param.src_height       != m_param.src_height)           return false;

    if (param.dst_pixel_format != m_param.dst_pixel_format)     return false;
    if (param.dst_width        != m_param.dst_width)            return false;
    if (param.dst_height       != m_param.dst_height)           return false;

    return true;
}

int Converter_FFmpeg::Initialize(const ConvertParam& param)
{
    if (false == IsEqualParam(param))
        Finalize();

    m_param = param;

    // Src avFrame 설정
    if (nullptr == m_avframe_src)
    {
        m_avframe_src = av_frame_alloc();
        if (nullptr == m_avframe_src)
            return 1;
    }

    // Dst avFrame 설정
    if (nullptr == m_avframe_dst)
    {
        m_avframe_dst = av_frame_alloc();
        if (nullptr == m_avframe_dst)
            return 1;
    }

    // SwsContext 설정
    if (nullptr == m_sws_contexts)
    {
        m_sws_contexts = sws_getContext(
            m_param.src_width, m_param.src_height, (AVPixelFormat)m_param.src_pixel_format, // Source
            m_param.dst_width, m_param.dst_height, (AVPixelFormat)m_param.dst_pixel_format, // Destination
            SWS_BICUBIC,//SWS_FAST_BILINEAR,
            nullptr, nullptr, nullptr);
        if (nullptr == m_sws_contexts)
            return 2;
    }

    return 0;
}

int Converter_FFmpeg::Finalize()
{
    if (m_avframe_src)
    {
        av_frame_free(&m_avframe_src);
        m_avframe_src = nullptr;
    }

    if (m_avframe_dst)
    {
        av_frame_free(&m_avframe_dst);
        m_avframe_dst = nullptr;
    }

    if (m_sws_contexts)
    {
        sws_freeContext(m_sws_contexts);
        m_sws_contexts = nullptr;
    }

    return 0;
}

int Converter_FFmpeg::Convert(const AVFrame* src_avframe, const ConvertParam& param)
{
    if (Initialize(param))
        return 1;

    if (param.dst_line_size > 0)
        m_avframe_dst->linesize[0] = param.dst_line_size;

    int ret = 0;
#ifdef __SUPPORT_FFMPEG_VER_3
    ret = avpicture_fill(
        reinterpret_cast<AVPicture*>(m_avframe_dst),
        param.dst_buffer,
        (AVPixelFormat)param.dst_pixel_format,
        param.dst_width, param.dst_height);
#else
    ret = av_image_fill_arrays(m_avframe_dst->data, m_avframe_dst->linesize,
        param.dst_buffer, (AVPixelFormat)param.dst_pixel_format, param.dst_width, param.dst_height, 1);
    if (ret < 0)
        return 1;
#endif
    ret = sws_scale(m_sws_contexts,                                        // sws_context
        src_avframe->data, src_avframe->linesize, 0, param.src_height,     // Source
        m_avframe_dst->data, m_avframe_dst->linesize);                     // Destination
    if (ret <= 0)
        return 2;

    return 0;
}

int Converter_FFmpeg::Convert(const uint8_t* src_buffer, const ConvertParam& param)
{
    if (Initialize(param))
        return 1;

    if (param.dst_line_size > 0)
        m_avframe_dst->linesize[0] = param.dst_line_size;

    int ret = 0;
#ifdef __SUPPORT_FFMPEG_VER_3
    ret = avpicture_fill(
        reinterpret_cast<AVPicture*>(m_avframe_src),
        src_buffer,
        (AVPixelFormat)param.src_pixel_format,
        param.src_width, param.src_height);
    if (ret < 0)
        return 1;

    ret = avpicture_fill(
        reinterpret_cast<AVPicture*>(m_avframe_dst),
        param.dst_buffer,
        (AVPixelFormat)param.dst_pixel_format,
        param.dst_width, param.dst_height);
    if (ret < 0)
        return 2;
#else
    ret = av_image_fill_arrays(m_avframe_src->data, m_avframe_src->linesize,
        src_buffer, (AVPixelFormat)param.src_pixel_format, param.src_width, param.src_height, 1);
    if (ret < 0)
        return 1;

    ret = av_image_fill_arrays(m_avframe_dst->data, m_avframe_dst->linesize,
        param.dst_buffer, (AVPixelFormat)param.dst_pixel_format, param.dst_width, param.dst_height, 1);
    if (ret < 0)
        return 1;
#endif
    ret = sws_scale(m_sws_contexts,                                           // sws_context
        m_avframe_src->data, m_avframe_src->linesize, 0, m_param.src_height,  // Source
        m_avframe_dst->data, m_avframe_dst->linesize);                        // Destination
    if (ret <= 0)
        return 3;

    return 0;
}

#pragma warning(pop)