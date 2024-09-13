#pragma once

#include <memory>
#include <vector>
#include <mutex>

class SendWorkerSyncQueue;
struct DecoderVideoDest;

class SendWorkerManager
{
private:
    using SendWorkerSyncQueuePtr = std::unique_ptr<SendWorkerSyncQueue>;

    struct SendWorkInfo
    {
        int frame_type = -1;
        int stream_index = -1;

        bool video_streaming = false;

        SendWorkerSyncQueuePtr  send_worker;
    };

    std::recursive_mutex        m_send_workers_mutex;
    std::vector<SendWorkInfo>   m_send_workers;

public:
    SendWorkerManager();
    ~SendWorkerManager();

    int  Activate();
    int  Deactivate();

    int  InsertSendWork(const std::string& queue_name, int sync_count);
    int  SetFrameInfo(const std::string& queue_name, int frame_type, int stream_index, bool video_streaming);

    void DeleteSendWork(const std::string& queue_name);
    bool IsExistSendWork(const std::string& queue_name);

    void UnlockSendWork();
    void UnlockSendWork(int stream_index);
    void ClearSendWork();

    int  SendStreamStatus(int stream_index, int status);
    int  SendFrame(const std::string& camera_guid, int stream_index, const DecoderVideoDest& data);
    int  SendProcessInfo(const std::string& queue_name);
};