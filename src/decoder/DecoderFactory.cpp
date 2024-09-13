#include "stdafx.h"
#include "DecoderFactory.h"
#include "DecoderDefine.h"
#include "Decoder_FFmpeg.h"

//////////////////////////////////////////////////////////////////////////
// class DecoderImpl
//////////////////////////////////////////////////////////////////////////

DecoderImpl::DecoderImpl()
{
    
}

DecoderImpl::~DecoderImpl()
{

}

void DecoderImpl::SetInfo(const DecoderInfo& param)
{
    m_info = param;
}

DecoderInfo& DecoderImpl::GetInfoRef()
{
    return m_info;
}

const DecoderInfo& DecoderImpl::GetInfo() const
{
    return m_info;
}

FrameInfo& DecoderImpl::GetFrameInfoRef(FRAME_TYPE type)
{
    return m_info.frame_info[type];
}

const FrameInfo& DecoderImpl::GetFrameInfo(FRAME_TYPE type) const
{
    return m_info.frame_info[type];
}

//////////////////////////////////////////////////////////////////////////
// class DecoderFactory
//////////////////////////////////////////////////////////////////////////

std::map<int, DecoderFactory::CreateCallback> DecoderFactory::m_mapSessions;

void DecoderFactory::Register(int type, CreateCallback callback)
{
    m_mapSessions[type] = callback;
}

void DecoderFactory::Unregister(int type)
{
    m_mapSessions.erase(type);
}

std::shared_ptr<DecoderImpl> DecoderFactory::CreateFactory(int type)
{
    auto it = m_mapSessions.find(type);
    if (it != m_mapSessions.end())
    {
        return (it->second)();
    }

    return std::shared_ptr<DecoderImpl>();
}

void DecoderFactory::Initialize()
{
    Register(DECODER_LIB_TYPE_FFMPEG, DecoderImpl_FFmpeg::Create);
}
