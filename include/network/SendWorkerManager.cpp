#include "stdafx.h"
#include "SendWorkerManager.h"
#include "SendWorkerSyncQueue.h"
#include "PacketDefine.h"
#include "PacketStruct.h"
#include "Application.h"

#include "common/Util.h"
#include "decoder/DecoderDefine.h"

//////////////////////////////////////////////////////////////////////////

SendWorkerManager::SendWorkerManager()
{
    
}

SendWorkerManager::~SendWorkerManager()
{
    Deactivate();
}

int SendWorkerManager::Activate()
{
    return 0;
}

int SendWorkerManager::Deactivate()
{
    ClearSendWork();

    return 0;
}

int SendWorkerManager::InsertSendWork(const std::string& queue_name, int sync_count)
{
    LOGI << fmt::format("Insert Send worker   QueueInfo[{} {}]", queue_name, sync_count);

    if (IsExistSendWork(queue_name))
    {
        LOGW << "Exist send work [" << queue_name << "]";
        return 1;
    }

    SendWorkInfo info;
    info.send_worker = std::make_unique<SendWorkerSyncQueue>();

    SendWorkerSyncQueue::ActivateParam send_init;
    send_init.queue_name = queue_name;
    send_init.sync_count = sync_count;
    if (info.send_worker->Activate(send_init))
    {
        LOGE << "Send worker activate failed [" << send_init.queue_name << "]";
        return 2;
    }

    std::lock_guard<std::recursive_mutex> lock(m_send_workers_mutex);
    m_send_workers.push_back(std::move(info));

    return 0;
}

int SendWorkerManager::SetFrameInfo(const std::string& queue_name, int frame_type, int stream_index, bool video_streaming)
{
    std::lock_guard<std::recursive_mutex> lock(m_send_workers_mutex);

    for (auto it = m_send_workers.begin() ; it != m_send_workers.end() ; it++)
    {
        SendWorkInfo& item = *it;
        if (queue_name == item.send_worker->GetQueueName())
        {
            item.frame_type      = frame_type;
            item.stream_index    = stream_index;
            item.video_streaming = video_streaming;
            break;
        }
    }

    return 0;
}

void SendWorkerManager::DeleteSendWork(const std::string& queue_name)
{
    std::lock_guard<std::recursive_mutex> lock(m_send_workers_mutex);

    for (auto it = m_send_workers.begin() ; it != m_send_workers.end() ; it++)
    {
        SendWorkInfo& item = *it;
        if (queue_name == item.send_worker->GetQueueName())
        {
            LOGI << fmt::format("Delete Send worker   FrameType[{}]  QueueInfo[{} {}]", item.frame_type, queue_name, item.send_worker->GetSyncCount());
            item.send_worker->Deactivate();
            m_send_workers.erase(it);
            break;
        }
    }
}

bool SendWorkerManager::IsExistSendWork(const std::string& queue_name)
{
    std::lock_guard<std::recursive_mutex> lock(m_send_workers_mutex);

    bool exist = false;
    for (SendWorkInfo& item : m_send_workers)
    {
        if (queue_name == item.send_worker->GetQueueName())
        {
            exist = true;
            break;
        }
    }
    return exist;
}

void SendWorkerManager::UnlockSendWork()
{
    std::lock_guard<std::recursive_mutex> lock(m_send_workers_mutex);

    for (SendWorkInfo& item : m_send_workers)
        item.send_worker->SendSyncUnlock();
}

void SendWorkerManager::UnlockSendWork(int stream_index)
{
    std::lock_guard<std::recursive_mutex> lock(m_send_workers_mutex);

    for (SendWorkInfo& item : m_send_workers)
    {
        if (item.stream_index == stream_index)
            item.send_worker->SendSyncUnlock();
    }
}

void SendWorkerManager::ClearSendWork()
{
    UnlockSendWork();

    std::lock_guard<std::recursive_mutex> lock(m_send_workers_mutex);
    m_send_workers.clear();
}

int SendWorkerManager::SendStreamStatus(int stream_index, int status)
{
    std::shared_ptr<Packet_StreamStatus> packet_status;
    packet_status = Packet_StreamStatus::Casting(PacketStruct::CreateFactory(PACKEK_TYPE_STREAM_STATUS));

    std::lock_guard<std::recursive_mutex> lock(m_send_workers_mutex);
    for (SendWorkInfo& item : m_send_workers)
    {
        packet_status->stream_index = stream_index;
        packet_status->status = status;
        item.send_worker->SendPacket(packet_status, 10);
    }

    return 0;
}

int SendWorkerManager::SendFrame(const std::string& camera_guid, int stream_index, const DecoderVideoDest& data)
{
    std::shared_ptr<Packet_Frame> packet_frame;
    packet_frame = Packet_Frame::Casting(PacketStruct::CreateFactory(PACKEK_TYPE_FRAME));

    packet_frame->camera_guid   = camera_guid;
    packet_frame->stream_index  = stream_index;
    packet_frame->order_num     = data.index;
    packet_frame->vcodec_id     = VCODEC_ID_H264;
    packet_frame->stream_width  = data.frame_info[FT_ORIGINAL].width;
    packet_frame->stream_height = data.frame_info[FT_ORIGINAL].height;

    std::lock_guard<std::recursive_mutex> lock(m_send_workers_mutex);
    for (SendWorkInfo& item : m_send_workers)
    {
        if (false == item.video_streaming)
            continue;
        if (item.stream_index != stream_index)
            continue;
        if (-1 == item.frame_type)
            continue;

        packet_frame->frame_type         = item.frame_type;
        packet_frame->frame_pixel_format = data.frame_info[item.frame_type].pixel_format;
        packet_frame->frame_width        = data.frame_info[item.frame_type].width;
        packet_frame->frame_height       = data.frame_info[item.frame_type].height;
        packet_frame->frame_buffer       = data.frame[item.frame_type].data;    // Decoder 에서 가지고 있는 Buffer 의 주소
        packet_frame->frame_buffer_len   = data.frame[item.frame_type].data_size;
        item.send_worker->SendPacket(packet_frame, 10);
    }

    return 0;
}

int SendWorkerManager::SendProcessInfo(const std::string& queue_name)
{
    std::lock_guard<std::recursive_mutex> lock(m_send_workers_mutex);

    int ret = 0;
    bool send = false;
    for (SendWorkInfo& item : m_send_workers)
    {
        if (queue_name == item.send_worker->GetQueueName())
        {
            std::string process_name = "PROCESS_CAMERA";
            Application::Instance()->ReadCmdLineValue(CMD_KEY_PROCESS_NAME, process_name);

            std::shared_ptr<Packet_ProcessInfo> packet_process_info;
            packet_process_info = Packet_ProcessInfo::Casting(PacketStruct::CreateFactory(PACKEK_TYPE_PROCESS_INFO));
            packet_process_info->process_name   = process_name;
            packet_process_info->process_id     = GetCurrentProcessId();
            packet_process_info->packet_version = APP_VERSION;

            ret = item.send_worker->SendPacket(packet_process_info, 100);
            if (0 == ret)
            {
                send = true;
                LOGI << fmt::format("Send Process Info  SendQuque[{}]  Name[{}]  PID[{}]  Version[{}]", queue_name, packet_process_info->process_name, packet_process_info->process_id, packet_process_info->packet_version);
            }
            break;
        }
    }

    if (false == send)
        LOGE << fmt::format("Send Process Info failed   Ret[{}] QueueName[{}]", ret, queue_name);

    return 0;
}