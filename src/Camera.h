#pragma once

// std c++
#include <memory>
#include <vector>
#include <mutex>

class Streamer;
class Controler;
class PTZ;
class RecvWorkerSyncQueue;
class CPacketReader;
class PacketStruct;
class RepeatWorkProc;
class SendWorkerManager;

struct StreamerInfo;
struct DecoderVideoDest;

class Camera
{
private:
    using StreamerPtr       = std::unique_ptr<Streamer>;
    using ControlerPtr      = std::unique_ptr<Controler>;
    using PTZPtr            = std::unique_ptr<PTZ>;

    std::vector<StreamerPtr>                m_streamers;
    ControlerPtr                            m_controler;
    PTZPtr                                  m_ptz;

    std::string                             m_camera_guid;
    int                                     m_camera_id = 0;
    RepeatWorkProc&                         m_repeqt_worker;

    std::unique_ptr<RecvWorkerSyncQueue>    m_recv_worker;
    std::unique_ptr<SendWorkerManager>      m_send_manager;

private:
    int  Initialize(const StreamerInfo& stream_info);

    // Queue repeat work function
    enum REPEAT_WORK_TYPE
    {
        CHECK_STREAM_0 = 0,
        CHECK_STREAM_1,
        CHECK_STREAM_2,
    };
    void OnTimerCheckStream(int stream_index);

    // Decoder thread 에서 호출
    int OnVideoDecompress(int stream_index, const DecoderVideoDest& data);
    
    // RecvWorkerSyncQueue thread 에서 호출
    void OnRecvWorker(const std::string& packet_name, CPacketReader* packet_reader);
    void OnRecvProcessInfoReq(std::shared_ptr<PacketStruct>& packet);
    void OnRecvStreamInfo(std::shared_ptr<PacketStruct>& packet);
    void OnRecvStreamRequest(std::shared_ptr<PacketStruct>& packet);

public:
    Camera();
    ~Camera();

    int Initialize();
    int Finalize();

    int Activate();
    int Deactivate();

    int LoadCamerasPbFile(const std::string& setup_file);
};