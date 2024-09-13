#pragma once

#include "DecoderFactory.h"

// std c++
#include <functional>
#include <memory>
#include <map>

struct AVFrame;
struct AVPacket;
struct AVCodec;
struct AVCodecContext;
struct DecoderInfo;

class Converter_FFmpeg;

//////////////////////////////////////////////////////////////////////////
// class DecoderImpl_FFmpeg
//////////////////////////////////////////////////////////////////////////

class DecoderImpl_FFmpeg : public DecoderImpl
{
private:
    AVFrame*            m_avframe         = nullptr;
    AVCodec*            m_avcodec         = nullptr;
    AVCodecContext*     m_avcodec_context = nullptr;
    AVPacket*           m_source_avpacket = nullptr;

    std::unique_ptr<Converter_FFmpeg>   m_convert;

public:
    DecoderImpl_FFmpeg();
    virtual ~DecoderImpl_FFmpeg();

    virtual int Activate() override;
    virtual int Deactivate() override;

    virtual int Decode(const VideoSource& src, DecoderVideoDest& dst) override;

    static std::shared_ptr<DecoderImpl_FFmpeg> Create()
    {
        return std::make_shared<DecoderImpl_FFmpeg>();
    }
};

