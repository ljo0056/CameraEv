#pragma once

// c++ stl
#include <string>
#include <functional>

//////////////////////////////////////////////////////////////////////////
//  define

enum SESSION_TYPE
{
    SESSION_TYPE_NONE = 0,
    SESSION_TYPE_FILE = 1,
    SESSION_TYPE_RTSP = 2,

    SESSION_TYPE_COUNT,
};

//////////////////////////////////////////////////////////////////////////
//  struct

struct SessionInfo
{
    std::string name;       // session 을 구별하기 위한 이름
    std::string url;
    std::string auth_id;
    std::string auth_pw;

    int  type       = SESSION_TYPE_NONE;
    int  port       = 0;
    bool use_tcp    = true;
};


struct SessionVideoFrame
{
    int   codec_id   = 0;
    int   s_buf_size = 0;

    byte* s_buffer   = nullptr;

    bool  key_frame  = false;

    timeval timestamp;
};


struct SessionAudioData
{
    int   codec_id   = 0;
    int   s_buf_size = 0;

    byte* s_buffer = nullptr;

    timeval timestamp;
};


struct SessionVideoInfo
{
    int width  = 0;
    int height = 0;
    float fps  = 0;
};


struct StreamCallback
{
    std::function<int (const SessionVideoFrame&)>   callback_video_frame_received;
    std::function<int (const SessionAudioData&)>    callback_audio_data_received;
    std::function<int (const SessionVideoInfo&)>    callback_video_info_changed;
};


struct ControlCallback
{

};


struct PTZCallback
{

};