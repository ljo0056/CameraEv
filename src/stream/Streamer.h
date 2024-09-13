#pragma once

#include <chrono>
#include <functional>
#include "decoder/DecoderDefine.h"
#include "session/SessionDefine.h"

class Session;
class Decoder;
class FPSChecker;
class PreviewWnd;

//////////////////////////////////////////////////////////////////////////
// Define
//////////////////////////////////////////////////////////////////////////

struct StreamerInfo
{
    int             stream_index = 0;
    bool            stream_used  = false;

    SessionInfo     session_info;
    DecoderInfo     decoder_info;

    std::function<int(int, const DecoderVideoDest&)> callback_decompress;
};

//////////////////////////////////////////////////////////////////////////
// class Streamer
//////////////////////////////////////////////////////////////////////////

class Streamer
{
private:
    using time_pt = std::chrono::high_resolution_clock::time_point;
    using time_ms = std::chrono::duration<double, std::milli>;

    int                             m_stream_index = -1;
    int                             m_print_log_fps = 0;
    std::shared_ptr<Session>        m_session;
    std::shared_ptr<Decoder>        m_decoder;
    std::unique_ptr<FPSChecker>     m_fps_checker;
    std::unique_ptr<PreviewWnd>     m_preview;

    std::function<int(int, const DecoderVideoDest&)> m_callback_decompress;

    bool m_initialize = false;
    bool m_activate   = false;

    time_pt m_push_frame_time;

private:
    // callback
    int OnVideoFrameReceived(const SessionVideoFrame& data);
    int OnVideoInfomationReceived(const SessionVideoInfo& data);
    int OnVideoDecompress(const DecoderVideoDest& data);

    void PrintFPS(double ms, int count, float fps);
    void SetCallback(std::shared_ptr<Session> session);

public:
    Streamer(int stream_index);
    virtual ~Streamer();

    int GetIndex() const;
    const SessionInfo& GetSessionInfo() const;

    int Initialize(const StreamerInfo& param);
    int Finalize();

    int Activate();
    int Deactivate();
    bool IsActivate() const;

    void SetDecodeFrameInfo(int type, const FrameInfo& info);
    int  GetIntervalPushFrameTime();
};
