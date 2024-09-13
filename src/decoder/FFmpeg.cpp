#include "stdafx.h"
#include "FFmpeg.h"
#include "DecoderDefine.h"
#include <array>

#pragma warning(push)
#ifdef __SUPPORT_FFMPEG_VER_3
#pragma warning(disable:4996)     // error C4996 의 deprecated 선언으로 인해 컴파일이 안되는 문제로 추가 (FFMPEG Ver3, Ver4 의 차이)
#endif
#pragma warning(disable:4819)     // libavutil/rational.h 의 코드 페이지(949) warning C4819 off (Line 49)

#pragma comment(lib, "ffmpeg/avcodec.lib")
#pragma comment(lib, "ffmpeg/avformat.lib")
#pragma comment(lib, "ffmpeg/avutil.lib")
#pragma comment(lib, "ffmpeg/swscale.lib")
#pragma comment(lib, "ffmpeg/swresample.lib")


void FFmpeg::Register()
{
    static bool init = false;

    if (false == init)
    {
#ifdef __SUPPORT_FFMPEG_VER_3
        avcodec_register_all();
        av_register_all();
#endif
        avformat_network_init();
        init = true;
        LOGI << "FFmpeg register.";
    }
}

std::string FFmpeg::ErrorCode(int code)
{
	std::array<char, AV_ERROR_MAX_STRING_SIZE> error_message{};
	av_strerror(code, error_message.data(), error_message.size());
	return std::string(" errCode[" + std::to_string(code) + "]: " + std::string(error_message.data()));
}

AVCodecID FFmpeg::GetAVCodecID(int vcodec_id)
{
	AVCodecID avcodec_id = AV_CODEC_ID_NONE;

	switch (vcodec_id)
	{
	case VCODEC_ID_H264:
		avcodec_id = AV_CODEC_ID_H264;
		break;
	default:
		break;
	}

	return avcodec_id;
}

int FFmpeg::GetToAVPixelFormat(int video_pixel_format)
{
	AVPixelFormat avpixel_format = AV_PIX_FMT_NONE;

	switch (video_pixel_format)
	{
	case VPIX_FMT_YUY2:
		avpixel_format = AV_PIX_FMT_YUYV422;
		break;
	case VPIX_FMT_YV12:
		avpixel_format = AV_PIX_FMT_YUV420P;
		break;
	case VPIX_FMT_BGR24:
		avpixel_format = AV_PIX_FMT_BGR24;
		break;
	case VPIX_FMT_RGB24:
		avpixel_format = AV_PIX_FMT_RGB24;
		break;
	default:
		break;
	}

	return avpixel_format;
}

int FFmpeg::GetImageBufferSize(int avpixel_format, int width, int height, int align)
{
    AVPixelFormat avpm = (AVPixelFormat)avpixel_format;
    return av_image_get_buffer_size(avpm, width, height, align);
}

int FFmpeg::GetFromAVPixelFormat(int avpixel_format)
{
    VIDEO_PIXEL_FORMAT video_pixel_format = VPIX_FMT_NONE;

    switch (avpixel_format)
    {
    case AV_PIX_FMT_YUYV422:
        video_pixel_format = VPIX_FMT_YUY2;
        break;
    case AV_PIX_FMT_YUV420P:
        video_pixel_format = VPIX_FMT_YV12;
        break;
    case AV_PIX_FMT_BGR24:
        video_pixel_format = VPIX_FMT_BGR24;
        break;
    case AV_PIX_FMT_RGB24:
        video_pixel_format = VPIX_FMT_RGB24;
        break;
    default:
        break;
    }

    return video_pixel_format;
}

#pragma warning(pop)
