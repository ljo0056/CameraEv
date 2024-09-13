#pragma once

#include <mutex>
#include <memory>
#include <thread>
#include <queue>
#include <chrono>

#include "DecoderDefine.h"

#include "common/InnerThread.h"

//////////////////////////////////////////////////////////////////////////
// class Decoder
//////////////////////////////////////////////////////////////////////////
/// class Decoder description
/// - 비디오 영상의 Decoding 을 담당하는 모듈 입니다.
/// - 다양한 외부 라이브러리를 이용하여 만들 수 있도록 Factory patten 으로 되어 있습니다.
/// - 주요 사용 라이브러리는 FFmpeg 이지만 간혹 CCTV 제조사에서 제공해주는 라이브러리가 사용될 여지가 있습니다.

class DecoderImpl;
class FPSChecker;
class QueueLocalMemory;

class Decoder : public InnerThread
{
private:
    using time_pt = std::chrono::high_resolution_clock::time_point;
    using time_ms = std::chrono::duration<double, std::milli>;

    struct QueueVideoSrc
    {
        VideoSource video;
        time_pt     time;
    };

    std::shared_ptr<DecoderImpl>        m_impl;

    std::recursive_mutex                m_queue_mutex;
    std::unique_ptr<QueueLocalMemory>   m_queue_stream; // 수신된 인코딩 영상을 저장 하는 Queue
    std::queue<QueueVideoSrc>           m_queue_src;    // 수신된 인코딩 영상의 정보를 저장하는 Queue
    std::unique_ptr<FPSChecker>         m_fps_checker;

    DecoderCallback                     m_callback;
    int                                 m_adaptive_fps = DECODE_DEFAULT_FPS;

    std::recursive_mutex                m_frame_info_mutex;

    bool    m_thread_running = false;

private:
    virtual void ThreadLoop() override;

    int  GetDecodingFps(int cur_fps);
    void PrintFPS(double ms, int count, float fps);
    bool IsFpsAuto();
    int  Decode(const VideoSource& src, DecoderVideoDest& dst);
    void SetDstBufferInfo(DecoderVideoDest& dst, std::vector<byte>* arr_buffer);
    bool SetAllocBuffer(DecoderVideoDest& dst, std::vector<byte>* arr_buffer);

public:
    Decoder();
    virtual ~Decoder();

    int Initialize(const DecoderInfo& param, const DecoderCallback& callback);
    int Finalize();

    int Activate();
    int Deactivate();

    const DecoderInfo& GetDecoderInfo() const;
    const FrameInfo& GetFrameInfo(FRAME_TYPE type) const;

    // FT_ORIGINAL 타입은 PixelFormat 만 설정할 수 있다.
    // Decoder 내부에서 원본 영상의 해상도를 설정 해준다.
    void SetFrameInfo(FRAME_TYPE type, const FrameInfo& info);
    int GetCalcBufferSize(int video_pixel_format, int width, int height) const;

    int QueuePush(const VideoSource& src);
};
