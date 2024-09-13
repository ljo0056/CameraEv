#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

class QueueSharedMemory;
class SyncSharedMemory;
class CPacketWriter;
class PacketStruct;

class SendWorkerSyncQueue
{
private:
    std::shared_ptr<QueueSharedMemory>  m_send_queue;
    std::unique_ptr<SyncSharedMemory>   m_send_sync;

    std::vector<uint8_t>                m_packet_buffer;
    std::unique_ptr<CPacketWriter>      m_packet_writer;

public:
    SendWorkerSyncQueue();
    ~SendWorkerSyncQueue();

    struct ActivateParam
    {
        std::string     queue_name;
        int             sync_count;
    };
    int  Activate(const ActivateParam& param);
    int  Deactivate();

    void SendSyncUnlock();
    std::string GetQueueName() const;
    int GetSyncCount() const;

    int SendPacket(std::shared_ptr<PacketStruct> packet, int ms = INFINITE);
    int SendPacket(std::function<void(CPacketWriter*)> func, int ms = INFINITE);
    int SendPacket(const std::string& packet_name, int len, std::function<void(void* data)> func, int ms = INFINITE);
};
