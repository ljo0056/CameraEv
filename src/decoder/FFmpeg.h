#pragma once

// FFMPEG API 사용 흐름을 Ver3.x 에 맞추어 사용할려고 할 시에 사용
// Ver3.x 과 Ver4.x 의 API 사용 흐름이 틀리다.
//#define __SUPPORT_FFMPEG_VER_3

#pragma warning(push)
#pragma warning(disable:4819)     // libavutil/rational.h 의 코드 페이지(949) warning C4819 off (Line 49)

extern "C"
{
#include <ffmpeg/libavcodec/avcodec.h>
#include <ffmpeg/libavformat/avformat.h>
#include <ffmpeg/libswscale/swscale.h>
#include <ffmpeg/libavutil/imgutils.h>
}

class FFmpeg 
{
public:
    static void Register();
    static std::string ErrorCode(int code);

    // VCODEC_ID -> AVCodecID 으로 변환
    // Return : 0 (AV_CODEC_ID_NONE) 이면 실패
    static AVCodecID GetAVCodecID(int vcodec_id);

    // VIDEO_PIXEL_FORMAT -> AVPixelFormat 으로 변환
    // Return : -1 (AV_PIX_FMT_NONE) 이면 실패
    static int GetToAVPixelFormat(int video_pixel_format);

    // AVPixelFormat -> VIDEO_PIXEL_FORMAT 으로 변환
    // Return : 0 (VPIX_FMT_NONE) 이면 실패
    static int GetFromAVPixelFormat(int avpixel_format);

    static int GetImageBufferSize(int avpixel_format, int width, int height, int align);
};

#pragma warning(pop)