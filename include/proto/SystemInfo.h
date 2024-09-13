#pragma once

//////////////////////////////////////////////////////////////////////////
// struct

struct StreamInfo
{
    std::string url = "rtsp://";
    std::string ip;
    std::string user_id;
    std::string user_pw;

    int         port = 554;
    int         stream_accumulate_time = 500;
    int         fps = 0;    // enum DECODE_FPS_TYPE

    bool        tcp = true;
    bool        stream_used = false;
};

struct CameraInfo
{
    enum 
    {
        NONE_ID = -1,
    };

    int         camera_id = NONE_ID; // 실행 될때 마다 생성되는 고유한 ID 이다. Save/Load 할 필요는 없다.
    std::string camera_file;
    std::string camera_guid;
    tstring     camera_name;

    StreamInfo  stream_info[3];
};

struct CameraWatchdog
{
    std::string guid;
    int pid = 0;
};

struct SystemInfo
{
    std::string system_guid;
    tstring     system_name;

    struct Camera
    {
        std::string guid;
    };

    std::vector<Camera>    cameras;
};

//////////////////////////////////////////////////////////////////////////
// api

int  SaveCameraInfoPB(const std::string& file_path, const CameraInfo& camera_info);
int  LoadCameraInfoPB(const std::string& file_path, CameraInfo& camera_info);

int  SaveSystemInfoPB(const std::string& file_path, const SystemInfo& system_info);
int  LoadSystemInfoPB(const std::string& file_path, SystemInfo& system_info);

int  SaveCameraWatchdog(const std::string& file_path, CameraWatchdog& watchdog_info);
int  LoadCameraWatchdog(const std::string& file_path, CameraWatchdog& watchdog_info);