#pragma once

#include "DecoderDefine.h"

struct AVFrame;
struct SwsContext;

//////////////////////////////////////////////////////////////////////////
// class Converter_FFmpeg
//////////////////////////////////////////////////////////////////////////

class Converter_FFmpeg final
{
public:
    struct ConvertParam
    {
        int src_pixel_format = 0;   // pixfmt.h 의 enum AVPixelFormat 참조
        int src_width, src_height;

        uint8_t* dst_buffer = nullptr;
        int dst_pixel_format = 0;   // pixfmt.h 의 enum AVPixelFormat 참조
        int dst_width, dst_height;

        // DIB 형식으로 컨버팅 하기 위해서는 linesize 를 조정해주어야함
        // int Margin = 0;
        // ((dst_width * 3 + Margin + 3) / 4 * 4);
        int dst_line_size = 0;
    };

private:
    AVFrame*            m_avframe_src;
    AVFrame*            m_avframe_dst;
    SwsContext*         m_sws_contexts;

    ConvertParam        m_param;

private:
    int  Initialize(const ConvertParam& param);
    int  Finalize();
    bool IsEqualParam(const ConvertParam& param);

public:
    Converter_FFmpeg();
    ~Converter_FFmpeg();

    int Convert(const AVFrame* src_avframe, const ConvertParam& param);
    int Convert(const uint8_t* src_avframe, const ConvertParam& param);
};

