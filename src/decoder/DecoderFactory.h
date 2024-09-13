#pragma once

// std c++
#include <functional>
#include <map>

#include "DecoderDefine.h"

//////////////////////////////////////////////////////////////////////////
// class DecoderImpl
//////////////////////////////////////////////////////////////////////////

class DecoderImpl
{
private:
    DecoderInfo    m_info;

public:
    DecoderImpl();
    virtual ~DecoderImpl();

    void SetInfo(const DecoderInfo& param);
    DecoderInfo& GetInfoRef();
    const DecoderInfo& GetInfo() const;

    FrameInfo& GetFrameInfoRef(FRAME_TYPE type);
    const FrameInfo& GetFrameInfo(FRAME_TYPE type) const;

    virtual int Activate() = 0;
    virtual int Deactivate() = 0;

    // return code : enum DecodeResult 참조
    virtual int Decode(const VideoSource& src, DecoderVideoDest& dst) = 0;

    static std::shared_ptr<DecoderImpl> Create()
    {
        return nullptr;
    }
};

//////////////////////////////////////////////////////////////////////////
// class DecoderFactory
//////////////////////////////////////////////////////////////////////////

class DecoderFactory
{
private:
    typedef std::function<std::shared_ptr<DecoderImpl>(void)> CreateCallback;
    static std::map<int, CreateCallback>   m_mapSessions;

public:
    static void Initialize();

    static void Register(int type, CreateCallback callback);
    static void Unregister(int type);

    static std::shared_ptr<DecoderImpl> CreateFactory(int type);
};

