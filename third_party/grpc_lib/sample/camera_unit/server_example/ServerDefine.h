#pragma once

#include "decoder/DecoderDefine.h"

//////////////////////////////////////////////////////////////////////////
//  define

enum SERVER_TYPE
{
    SERVER_TYPE_NONE   = 0,
};


//////////////////////////////////////////////////////////////////////////
//  struct

struct ServerInfo
{
    int type        = SERVER_TYPE_NONE;
    int port_number = 0;

    std::string ip_address;
};

struct ServerVideoStreamInfo
{
    int stream_id    = 0;

    int width        = 0;
    int height       = 0;
    int pixel_format = VPIX_FMT_NONE;
    int vcodec_id    = VCODEC_ID_NONE;

    float fps        = 0;
};

struct ServerVideoStreamData
{
    int stream_index = 0;

    uint8_t* buffer = nullptr;
    int      buffer_len = 0;
};



