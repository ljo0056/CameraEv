#include "stdafx.h"
#include "SystemInfo.h"

#pragma warning(push)
#pragma warning(disable:4244)

#include <fstream>
#include <vector>

#include "common/Util.h"
#include "proto/CameraInfoFile.pb.h"
#include "decoder/DecoderDefine.h"


int SaveCameraInfoPB(const std::string& file_path, const CameraInfo& camera_info)
{
    CameraInfoFile::Camera camera_pb;
    camera_pb.set_camera_guid(camera_info.camera_guid);
    camera_pb.set_camera_name(convert_unicode_to_utf8(camera_info.camera_name));

    for (int ii = 0 ; ii < 3 ; ii++)
    {
        const StreamInfo& stream_info = camera_info.stream_info[ii];
        CameraInfoFile::Camera_StreamInfo* stream_node = camera_pb.add_stream_info();

        stream_node->set_url(stream_info.url);
        stream_node->set_ip(stream_info.ip);
        stream_node->set_port(stream_info.port);
        stream_node->set_user_id(stream_info.user_id);
        stream_node->set_user_pw(stream_info.user_pw);
        stream_node->set_tcp(stream_info.tcp);
        stream_node->set_stream_accumulate_time(stream_info.stream_accumulate_time);
        stream_node->set_fps(stream_info.fps);
        stream_node->set_stream_used(stream_info.stream_used);
    }

    std::fstream output(file_path, std::ios::out | std::ios::trunc | std::ios::binary);
    if (false == camera_pb.SerializeToOstream(&output))
        return 1;

    return 0;
}

int LoadCameraInfoPB(const std::string& file_path, CameraInfo& camera_info)
{
    if (false == IsExistFile(file_path))
        return 1;

    CameraInfoFile::Camera camera_pb;
    std::fstream input(file_path, std::ios::in | std::ios::binary);
    if (false == camera_pb.ParseFromIstream(&input))
        return 2;

    camera_info.camera_guid = camera_pb.camera_guid();
    camera_info.camera_name = convert_utf8_to_unicode(camera_pb.camera_name());

    for (int ii = 0 ; ii < 3 ; ii++)
    {
        StreamInfo& stream_info = camera_info.stream_info[ii];
        const CameraInfoFile::Camera_StreamInfo& stream_node = camera_pb.stream_info(ii);
        stream_info.url = stream_node.url();
        stream_info.ip  = stream_node.ip();
        stream_info.port = stream_node.port();
        stream_info.user_id = stream_node.user_id();
        stream_info.user_pw = stream_node.user_pw();
        stream_info.tcp = stream_node.tcp();
        stream_info.stream_accumulate_time = stream_node.stream_accumulate_time();
        stream_info.fps = stream_node.fps();
        stream_info.stream_used = stream_node.stream_used();
    }

    return 0;
}

int SaveSystemInfoPB(const std::string& file_path, const SystemInfo& system_info)
{
    CameraInfoFile::System system_pb;
    system_pb.set_system_guid(system_info.system_guid);
    system_pb.set_system_name(convert_unicode_to_utf8(system_info.system_name));

    for (const SystemInfo::Camera& camera : system_info.cameras)
    {
        CameraInfoFile::System_CameraInfo* camera_node = system_pb.add_cameras();
        camera_node->set_camera_guid(camera.guid);
    }

    std::fstream output(file_path, std::ios::out | std::ios::trunc | std::ios::binary);
    if (false == system_pb.SerializeToOstream(&output))
        return 1;

    return 0;
}

int LoadSystemInfoPB(const std::string& file_path, SystemInfo& system_info)
{
    if (false == IsExistFile(file_path))
        return 1;

    CameraInfoFile::System system_pb;
    std::fstream input(file_path, std::ios::in | std::ios::binary);
    if (false == system_pb.ParseFromIstream(&input))
        return 2;

    system_info.system_guid = system_pb.system_guid();
    system_info.system_name = convert_utf8_to_unicode(system_pb.system_name());

    system_info.cameras.clear();
    for (int ii = 0 ; ii < system_pb.cameras_size() ; ii++)
    {
        const CameraInfoFile::System_CameraInfo& camera_node = system_pb.cameras(ii);

        SystemInfo::Camera camera;
        camera.guid = camera_node.camera_guid();

        system_info.cameras.push_back(camera);
    }

    return 0;
}

int SaveCameraWatchdog(const std::string& file_path, CameraWatchdog& watchdog_info)
{
    CameraInfoFile::CameraWatchdog watchdog_pb;

    watchdog_pb.set_pid(watchdog_info.pid);
    std::fstream output(file_path, std::ios::out | std::ios::trunc | std::ios::binary);
    if (false == watchdog_pb.SerializeToOstream(&output))
        return 1;

    return 0;
}

int LoadCameraWatchdog(const std::string& file_path, CameraWatchdog& watchdog_info)
{
    if (false == IsExistFile(file_path))
        return 1;

    CameraInfoFile::CameraWatchdog watchdog_pb;
    std::fstream input(file_path, std::ios::in | std::ios::binary);
    if (false == watchdog_pb.ParseFromIstream(&input))
        return 2;

    watchdog_info.pid = watchdog_pb.pid();

    return 0;
}

#pragma warning(pop)