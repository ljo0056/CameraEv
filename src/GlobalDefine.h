#pragma once

#include <string>

#define APP_VERSION     ("1.0.0.1")

#ifdef _DEBUG
#define __SUPPORT_PREVIEW_WINDOW
#define __SUPPORT_SAMPLE_RTSP
#endif

#ifdef _UNICODE
    #ifndef _T
        #define _T(x)      L ## x
    #endif

    typedef std::basic_string<wchar_t> tstring;
#else
    #ifndef _T
        #define _T(x)       x
    #endif

    typedef std::basic_string<char> tstring;
#endif

// 프로그램 종료 코드
enum APP_ERROR_CODE
{
    APP_SUCCESS = 0,
    
    APP_EXIST_PROCESS_NAME,
    APP_ONCE_EVENT_FAILED,
};

// class CmdLineParser 에서 사용하는 Key 값을 정의
#define CMD_KEY_PROCESS_NAME     ("PROCESS_NAME")
#define CMD_KEY_CAMERA_ID        ("CAMERA_ID")
#define CMD_KEY_CAMERA_GUID      ("CAMERA_GUID")
#define CMD_KEY_RTSP             ("RTSP")