#pragma once

#include <memory>
#include <string>
#include <functional>
#include <vector>

#include "common/InnerThread.h"

class QueueSharedMemory;
class SyncSharedMemory;
class CPacketReader;

class RecvWorkerSyncQueue : public InnerThread
{
private:
    using CallbackReader = std::function<void(const std::string& packet_name, CPacketReader* packet_reader)>;

    bool m_thread_running = false;

    std::shared_ptr<QueueSharedMemory>  m_recv_queue;
    std::unique_ptr<SyncSharedMemory>   m_recv_sync;

    std::vector<uint8_t>                m_packet_buffer;
    std::unique_ptr<CPacketReader>      m_packet_reader;

    CallbackReader                      m_callback_reader;

private:
    bool CallbackConsumer(std::shared_ptr<QueueSharedMemory>& queue);

    virtual void ThreadLoop() override;

public:
    RecvWorkerSyncQueue();
    ~RecvWorkerSyncQueue();

    struct ActivateParam
    {
        std::string     queue_name;
        int             sync_count;
        bool            queue_clear = false;
        CallbackReader  callback_reader;
    };
    int  Activate(const ActivateParam& data);
    int  Deactivate();
    int  ClearQueue();

    std::string GetQueueName() const;
    int GetSyncCount() const;
};