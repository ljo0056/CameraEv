#include "stdafx.h"
#include "Camera.h"
#include "Application.h"

#include <functional>

#include "stream/Streamer.h"
#include "ptz/PTZ.h"
#include "control/Control.h"

#include "common/Util.h"
#include "common/TimerLockerManager.h"
#include "common/RepeatWorkProc.h"

#include "network/PacketDefine.h"
#include "network/PacketStruct.h"
#include "network/RecvWorkerSyncQueue.h"
#include "network/SendWorkerManager.h"

#include "proto/SystemInfo.h"


Camera::Camera()
    : m_recv_worker(std::make_unique<RecvWorkerSyncQueue>())
    , m_send_manager(std::make_unique<SendWorkerManager>())
    , m_repeqt_worker(RepeatWorkProc::GetInstance())
{
    m_streamers.reserve(3);
}

Camera::~Camera()
{

}

int Camera::Initialize()
{
    Application::Instance()->ReadCmdLineValue(CMD_KEY_CAMERA_ID, m_camera_id);
    Application::Instance()->ReadCmdLineValue(CMD_KEY_CAMERA_GUID, m_camera_guid);

    for (int ii = 0 ; ii < 3 ; ii++)
        m_streamers.push_back(std::make_unique<Streamer>(ii));

    return 0;
}

int Camera::Finalize()
{
    for (StreamerPtr& streamer : m_streamers)
        streamer->Finalize();
    m_streamers.clear();

    return 0;
}

int Camera::Activate()
{
    RecvWorkerSyncQueue::ActivateParam recv_init;
    recv_init.sync_count = 3;
    recv_init.queue_name = fmt::sprintf("CAMERA_QUEUE_%s", m_camera_guid);
    recv_init.callback_reader = [this](const std::string& packet_name, CPacketReader* packet_reader) {
        OnRecvWorker(packet_name, packet_reader);
    };
    if (m_recv_worker->Activate(recv_init))
        LOGE << "Recv worker activate failed [" << m_camera_guid << "]";
    if (m_send_manager->Activate())
        LOGE << "Send Manager activate failed [" << m_camera_guid << "]";

#ifdef __SUPPORT_SAMPLE_RTSP
    std::string rtsp_url;
    Application::Instance()->ReadCmdLineValue(CMD_KEY_RTSP, rtsp_url);

    if (false == rtsp_url.empty())
    {
        StreamerInfo stream_info;
        stream_info.stream_index = 0;
        stream_info.session_info.name = "camera 01";
        stream_info.session_info.auth_id = "admin";
        stream_info.session_info.auth_pw = "admin";
        stream_info.session_info.type = SESSION_TYPE_RTSP;
        stream_info.session_info.port = 554;
        stream_info.session_info.url = rtsp_url;
        //stream_info.session_info.url = "rtsp://127.0.0.1/ch1/stream1/media.imp"; // VLC 를 이용하여 RTSP 를 스트리밍 테스트를 할 수 있다.
        stream_info.callback_decompress = [this](int stream_index, const DecoderVideoDest& data) -> int {
            return OnVideoDecompress(stream_index, data);
        };

        stream_info.decoder_info.vcodec_id = VCODEC_ID_H264;
        stream_info.decoder_info.frame_info[FT_DISPLAY].pixel_format = VPIX_FMT_BGR24;
        stream_info.decoder_info.fps = 30;

        Initialize(stream_info);
        StreamerPtr& streamer = m_streamers[stream_info.stream_index];
        streamer->Activate();

        FrameInfo frame_info;

        if (streamer->IsActivate())
        {
            frame_info.pixel_format = VPIX_FMT_BGR24;
            frame_info.decode = true;
            frame_info.width = 640;
            frame_info.height = 480;
            streamer->SetDecodeFrameInfo(FT_DISPLAY, frame_info);
        }

        m_repeqt_worker.AddWork(
            CHECK_STREAM_0 + stream_info.stream_index,
            1000 * 10,
            std::bind(&Camera::OnTimerCheckStream, this, stream_info.stream_index));
    }    
#endif

    m_repeqt_worker.Activate();

    return 0;
}

int Camera::Deactivate()
{
    LOGI << "Deactivate start...";

    m_send_manager->Deactivate();
    m_repeqt_worker.Deactivate();
    m_recv_worker->Deactivate();

    for (StreamerPtr& streamer : m_streamers)
        streamer->Deactivate();

    LOGI << "Deactivate end...";
    return 0;
}

int Camera::Initialize(const StreamerInfo& stream_info)
{
    StreamerPtr& streamer = m_streamers[stream_info.stream_index];
    int ret = streamer->Initialize(stream_info);
    if (ret)
    {
        streamer.reset();
        return ret;
    }

    return 0;
}

int Camera::LoadCamerasPbFile(const std::string& setup_file)
{
    CameraInfo camera_info;
    camera_info.camera_file = setup_file;
    if (LoadCameraInfoPB(camera_info.camera_file, camera_info))
        return 1;

    for (int ii = 0 ; ii < 3 ; ii++)
    {
        StreamerPtr& streamer    = m_streamers[ii];
        StreamInfo&  stream_info = camera_info.stream_info[ii];
        StreamerInfo streamer_init;

        // sesseion
        streamer_init.session_info.name    = fmt::format("Rtsp  stream index[{}] Url[{}]", ii, stream_info.url);
        streamer_init.session_info.auth_id = stream_info.user_id;
        streamer_init.session_info.auth_pw = stream_info.user_pw;
        streamer_init.session_info.type    = SESSION_TYPE_RTSP;
        streamer_init.session_info.port    = stream_info.port;
        streamer_init.session_info.url     = stream_info.url;
        streamer_init.session_info.use_tcp = stream_info.tcp;

        // decoder
        streamer_init.decoder_info.fps = stream_info.fps;

        // stream
        streamer_init.stream_index = ii;
        streamer_init.stream_used  = stream_info.stream_used;
        streamer_init.callback_decompress = [this](int stream_index, const DecoderVideoDest& data) -> int {
            return OnVideoDecompress(stream_index, data);
        };

        if (stream_info.stream_used)
        {
            m_send_manager->UnlockSendWork(streamer_init.stream_index); // OnVideoDecompress 에서 대기중인 Block 을 풀어줌

            streamer->Deactivate();
            streamer->Finalize();

            streamer->Initialize(streamer_init);
            streamer->Activate();

            m_send_manager->SendStreamStatus(streamer_init.stream_index, Packet_StreamStatus::READY);

            m_repeqt_worker.AddWork(
                CHECK_STREAM_0 + streamer_init.stream_index,
                1000 * 10,
                std::bind(&Camera::OnTimerCheckStream, this, streamer_init.stream_index));

            LOGI << fmt::format("Stream activate  Index[{}] URL[{}] FPS[{}]", streamer_init.stream_index, stream_info.url, stream_info.fps);
        }
    }

    return 0;
}

int Camera::OnVideoDecompress(int stream_index, const DecoderVideoDest& data)
{
    m_send_manager->SendFrame(m_camera_guid, stream_index, data);

    return 0;
}

void Camera::OnRecvWorker(const std::string& packet_name, CPacketReader* packet_reader)
{
    std::shared_ptr<PacketStruct> packet = PacketStruct::CreateFactory(packet_name);
    packet->Reader(packet_reader);

    switch (packet->PacketType())
    {
    case PACKEK_TYPE_PROCESS_INFO_REQ:
        OnRecvProcessInfoReq(packet);
        break;
    case PACKEK_TYPE_STREAM_INFO:
        OnRecvStreamInfo(packet);
        break;
    case PACKEK_TYPE_STREAM_REQUEST:
        OnRecvStreamRequest(packet);
        break;
    default:
        LOGW << fmt::format("Could not Packet ...  Name[{0}]", packet_name);
        break;
    }
}

void Camera::OnRecvProcessInfoReq(std::shared_ptr<PacketStruct>& packet)
{
    Packet_ProcessInfoReqPtr packet_process_info_req = Packet_ProcessInfoReq::Casting(packet);

    LOGI << fmt::format("Recv ProcessInfoReq  QueueInfo[{} {}]", packet_process_info_req->queue_name, packet_process_info_req->sync_count);

    m_send_manager->InsertSendWork(packet_process_info_req->queue_name, packet_process_info_req->sync_count);
    m_send_manager->SendProcessInfo(packet_process_info_req->queue_name);
}

void Camera::OnRecvStreamInfo(std::shared_ptr<PacketStruct>& packet)
{
    Packet_StreamInfoPtr packet_stream_info = Packet_StreamInfo::Casting(packet);

    LOGI << fmt::format("Recv StreamInfo  start  Info[{}]", packet_stream_info->setup_file);
    LoadCamerasPbFile(packet_stream_info->setup_file);
    LOGI << fmt::format("Recv StreamInfo  end  Info[{}]", packet_stream_info->setup_file);
}

void Camera::OnRecvStreamRequest(std::shared_ptr<PacketStruct>& packet)
{
    Packet_StreamRequestPtr packet_stream_request = Packet_StreamRequest::Casting(packet);

    LOGI << fmt::format("Recv StreamRequest  Streaming[{}]  StreamIndex[{}]  FrameType[{}]  Format[{}]  Resolution[{}x{}]",
        packet_stream_request->video_streaming,
        packet_stream_request->stream_index,
        packet_stream_request->frame_type,
        packet_stream_request->pixel_format,
        packet_stream_request->width, packet_stream_request->height);

    StreamerPtr& streamer = m_streamers[packet_stream_request->stream_index];
    FrameInfo frame_info;

    if (streamer->IsActivate())
    {
        frame_info.pixel_format = packet_stream_request->pixel_format;
        frame_info.width  = packet_stream_request->width;
        frame_info.height = packet_stream_request->height;
        frame_info.decode = packet_stream_request->video_streaming;
        streamer->SetDecodeFrameInfo(packet_stream_request->frame_type, frame_info);
    }

    m_send_manager->SetFrameInfo(packet_stream_request->queue_name,
        packet_stream_request->frame_type,
        packet_stream_request->stream_index,
        packet_stream_request->video_streaming);

    m_send_manager->UnlockSendWork(packet_stream_request->stream_index);
}

void Camera::OnTimerCheckStream(int stream_index)
{
    StreamerPtr& streamer = m_streamers[stream_index];
    
    if (false == streamer->IsActivate())
        return;

    if (streamer->GetIntervalPushFrameTime() < 3000)
        return;

    streamer->Deactivate();
    streamer->Activate();

    m_send_manager->SendStreamStatus(stream_index, Packet_StreamStatus::NO_SIGNAL);
    m_send_manager->SendStreamStatus(stream_index, Packet_StreamStatus::RECONNECT);

    LOGI << fmt::format("No Signal Stream  StreamIndex[{}] URL[{}]", streamer->GetIndex(), streamer->GetSessionInfo().url);
}

