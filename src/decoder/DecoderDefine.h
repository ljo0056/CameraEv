#pragma once

#include <string>
#include <array>
#include <functional>

//////////////////////////////////////////////////////////////////////////
//  define

enum DECODER_LIB_TYPE
{
    DECODER_LIB_TYPE_NONE   = 0,
    DECODER_LIB_TYPE_FFMPEG = 1,
};

enum VCODEC_ID
{
    VCODEC_ID_NONE = 0,
    VCODEC_ID_H264 = 1
};

enum VIDEO_PIXEL_FORMAT
{
    VPIX_FMT_NONE  = 0,
    VPIX_FMT_YUY2  = 1,
    VPIX_FMT_YV12  = 2,
    VPIX_FMT_BGR24 = 3,
    VPIX_FMT_RGB24 = 4,
};

enum DECODE_RESULT
{
    DECODE_SUCCESS              = 0,
    DECODE_FAILED               = 1,
    DECODE_NEED_MORE_FRAME      = 2,
    DECODE_CONVERT_INIT_FAILED  = 3,
    DECODE_CONVERT_FAILED       = 4,
};

enum DECODE_FPS_TYPE
{
    DECODE_FPS_AUTO = 0,
    DECODE_DEFAULT_FPS = 30,
};

enum FRAME_TYPE
{
    FT_ORIGINAL = 0,
    FT_DISPLAY,
    FT_RECODE,
    FT_NUM,
};

//////////////////////////////////////////////////////////////////////////
//  struct

struct FrameBuffer
{
    byte* data = nullptr;
    int   data_size = 0;
};

struct FrameInfo
{
    int pixel_format = VPIX_FMT_NONE;
    int width  = 0;
    int height = 0;
    bool decode = false;
};

struct DecoderInfo
{
    int type         = DECODER_LIB_TYPE_FFMPEG;

    int vcodec_id    = VCODEC_ID_H264;

    // Decoding frame 의 이미지 정보 설정 
    FrameInfo  frame_info[FT_NUM];

    int queue_accumulate_time = 1000;   // Stream Source 적재 시간 (ms)
    int fps = DECODE_FPS_AUTO;          // DECODE_FPS_AUTO 초과 값을 갖고 있다면 해당 fps 사용 ex) fps=30 이면 수동으로 fps 30 으로 설정
};

struct VideoSource
{
    FrameBuffer frame;

    bool  key_frame  = false;

    timeval timestamp;           // 영상 수신 SDK 에서 넘겨주는 시간  ex) Live555
};

struct DecoderVideoDest
{
    int   index  = 0;    // decoding 된 frame 순서
    int   fps    = 0;    // DECODE_FPS_AUTO 설정 되어 있다면 실제 분석된 디코딩 fps 값이 설정 된다.

    // DecoderInfo 의 설정 정보
    FrameBuffer frame[FT_NUM];
    FrameInfo   frame_info[FT_NUM];
};

struct DecoderCallback
{
    std::function<int(const DecoderVideoDest&)>    callback_decompress;
};
