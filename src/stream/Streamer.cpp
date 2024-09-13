#include "stdafx.h"
#include "Streamer.h"
#include "Application.h"

#include <chrono>

#include "session/Session.h"
#include "session/SessionFactory.h"

#include "decoder/Decoder.h"

#include "common/FPSChecker.h"
#include "common/PreviewWnd.h"



//////////////////////////////////////////////////////////////////////////
// plog
//////////////////////////////////////////////////////////////////////////

namespace plog
{
    Record& operator<<(Record& record, const SessionVideoFrame& video_frame)
    {
        std::string msg = fmt::format(" VideoFrame   codec_id[{0}] buf_size[{1}] key_frame[{2}]",
            video_frame.codec_id,
            video_frame.s_buf_size,
            video_frame.key_frame
        );

        return record << msg;
    }

    Record& operator<<(Record& record, const SessionVideoInfo& video_info)
    {
        std::string msg = fmt::format(" VideoInfomaiton   width[{0}] height[{1}] ",
            video_info.width,
            video_info.height
        );

        return record << msg;
    }
}

//////////////////////////////////////////////////////////////////////////
// Streamer
//////////////////////////////////////////////////////////////////////////

Streamer::Streamer(int stream_index)
    : m_stream_index(stream_index)
    , m_fps_checker(new FPSChecker)
    , m_preview(new PreviewWnd)
{
    SessionFactory::Initialize();
}

Streamer::~Streamer()
{

}

int Streamer::GetIndex() const
{
    return m_stream_index;
}

const SessionInfo& Streamer::GetSessionInfo() const
{
    return m_session->GetSessionInfo();
}

void Streamer::PrintFPS(double ms, int count, float fps)
{
    //if (0 == m_print_log_fps % 30 || m_print_log_fps < 10)
        LOGI << fmt::format("Decompress fps   Time[{0:.1f}] count[{1}] FPS[{2:.1f}]", ms, count, fps);
    m_print_log_fps++;
}

void Streamer::SetCallback(std::shared_ptr<Session> session)
{
    StreamCallback stream_callback;
    stream_callback.callback_video_frame_received = [this](const SessionVideoFrame& data) -> int {
        return OnVideoFrameReceived(data);
    };
    stream_callback.callback_video_info_changed = [this](const SessionVideoInfo& data) -> int {
        return OnVideoInfomationReceived(data);
    };

    session->SetStreamCallback(stream_callback);
}

int Streamer::Initialize(const StreamerInfo& param)
{
    LOGI << "Initialize start stream_index[" << param.stream_index << "]";

    if (m_initialize)
        return 1;
    if (m_activate)
        return 2;

    m_stream_index = param.stream_index;
    m_callback_decompress = param.callback_decompress;

    // Decoder Initialize
    {
        DecoderCallback callback;
        callback.callback_decompress = [this](const DecoderVideoDest& data) -> int {
            OnVideoDecompress(data);
            return 0;
        };

        m_decoder = std::make_shared<Decoder>();
        if (m_decoder->Initialize(param.decoder_info, callback))
            return 3;
    }

    // Session Initialize
    {
        m_session = SessionFactory::CreateFactory(param.session_info.type);
        m_session->SetSessionInfo(param.session_info);
        SetCallback(m_session);

        if (m_session->Initialize())
            return 4; 

#ifdef __SUPPORT_PREVIEW_WINDOW
        m_preview->Initialize();
#endif
    }

    m_initialize = true;

    LOGI << "Initialize end stream_index[" << param.stream_index << "]";

    return 0;
}

int Streamer::Finalize()
{
    LOGI << "Finalize start stream_index[" << m_stream_index << "]";

    if (m_session)
        m_session->Finalize();

#ifdef __SUPPORT_PREVIEW_WINDOW
    if (m_preview)
        m_preview->Finalize();
#endif

    if (m_decoder)
        m_decoder->Finalize();

    m_initialize = false;

    LOGI << "Finalize end stream_index[" << m_stream_index << "]";

    return 0;
}

int Streamer::Activate()
{
    LOGI << "Activate start stream_index[" << m_stream_index << "]";

    if (false == m_initialize)
        return 1;

    if (m_session->Activate())
        return 2;

    if (m_decoder->Activate())
        return 3;
#ifdef __SUPPORT_PREVIEW_WINDOW
    m_preview->Activate();
#endif

    m_activate = true;
    m_push_frame_time = std::chrono::high_resolution_clock::now();

    LOGI << "Activate end stream_index[" << m_stream_index << "]";

    return 0;
}

int Streamer::Deactivate()
{
    LOGI << fmt::format("Deactivate start stream_index[{}] init[{}] activate[{}]", m_stream_index, m_initialize, m_activate);

    if (false == m_initialize)
        return 1;

    m_session->Deactivate();
    m_decoder->Deactivate();

#ifdef __SUPPORT_PREVIEW_WINDOW
    m_preview->Deactivate();
#endif
    m_print_log_fps = 0;
    m_activate = false;

    LOGI << "Deactivate end stream_index[" << m_stream_index << "]";
    return 0;
}

bool Streamer::IsActivate() const
{
    return m_activate;
}

void Streamer::SetDecodeFrameInfo(int type, const FrameInfo& info)
{
    LOGI << fmt::format("Set Decode Frame Info  Type[{}] Format[{}] Resoultion[{}x{}]", type, info.pixel_format, info.width, info.height);
    m_decoder->SetFrameInfo((FRAME_TYPE)type, info);
}

int Streamer::GetIntervalPushFrameTime()
{
    time_pt cur_time = std::chrono::high_resolution_clock::now();
    time_ms elapsed  = cur_time - m_push_frame_time;

    return (int)elapsed.count();
}

int Streamer::OnVideoFrameReceived(const SessionVideoFrame& data)
{
    //LOGI << m_session->GetSessionInfo().name << data;

    VideoSource src;
    src.frame.data = data.s_buffer;
    src.frame.data_size = data.s_buf_size;
    src.key_frame  = data.key_frame;
    src.timestamp  = data.timestamp;

    m_decoder->QueuePush(src);
    m_push_frame_time = std::chrono::high_resolution_clock::now();

    return 0;
}

int Streamer::OnVideoDecompress(const DecoderVideoDest& data)
{
    m_fps_checker->DoCheckCount([this](double ms, int count, float fps) {
        PrintFPS(ms, count, fps);
    });

    if (m_callback_decompress)
        m_callback_decompress(m_stream_index, data);

#ifdef __SUPPORT_PREVIEW_WINDOW
    m_preview->DrawImage(data.frame[FT_DISPLAY].data,
        data.frame_info[FT_DISPLAY].width, data.frame_info[FT_DISPLAY].height);
#endif

    return 0;
}

int Streamer::OnVideoInfomationReceived(const SessionVideoInfo& data)
{
    LOGI << m_session->GetSessionInfo().name << data;

    return 0;
}

