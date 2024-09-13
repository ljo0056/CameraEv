#include "stdafx.h"
#include "SendWorkerSyncQueue.h"

#include "common/Util.h"
#include "common/QueueSharedMemory.h"
#include "common/SyncSharedMemory.h"
#include "PacketDefine.h"
#include "PacketStruct.h"

//////////////////////////////////////////////////////////////////////////

SendWorkerSyncQueue::SendWorkerSyncQueue()
    : m_send_queue(std::make_shared<QueueSharedMemory>())
    , m_send_sync(std::make_unique<SyncSharedMemory>())
    , m_packet_writer(std::make_unique<CPacketWriter>())
    , m_packet_buffer(1024 * 1024 * 3)
{
    m_packet_writer->SetBuffer(&m_packet_buffer);
}

SendWorkerSyncQueue::~SendWorkerSyncQueue()
{
    Deactivate();
}

int SendWorkerSyncQueue::Activate(const ActivateParam& param)
{
    // Init QueueSharedMemory
    if (m_send_queue->Initialize(param.queue_name, 1024 * 1024 * 16))
        return 1;

    // Init SyncSharedMemory
    SyncSharedMemory::ActivateParam sync_init;
    sync_init.producer_count = param.sync_count;
    sync_init.consumer_count = param.sync_count;
    sync_init.queue          = m_send_queue;
    if (m_send_sync->Activate(sync_init))
        return 2;

    m_packet_buffer.resize(1024 * 1024 * 3);

    return 0;
}

int SendWorkerSyncQueue::Deactivate()
{
    m_send_sync->Deactivate();
    m_send_queue->Finalize();

    return 0;
}

void SendWorkerSyncQueue::SendSyncUnlock()
{
    m_send_sync->SendSyncUnlock();
}

std::string SendWorkerSyncQueue::GetQueueName() const
{
    return m_send_queue->GetName();
}

int SendWorkerSyncQueue::GetSyncCount() const
{
    return m_send_sync->GetConsumerCount();
}

int SendWorkerSyncQueue::SendPacket(std::shared_ptr<PacketStruct> packet, int ms)
{
    packet->Writer(m_packet_writer.get());

    auto func_producer = [this](std::shared_ptr<QueueSharedMemory> queue) -> bool
    {
        int ret = queue->Push(m_packet_writer->GetBufferPtr(), (uint32_t)m_packet_writer->GetPos());
        return 0 == ret ? true : false;
    };

    int ret = m_send_sync->SyncProducer(func_producer, ms);
    if (ret && INFINITE == ms) {
        LOGE << fmt::format("Send worker SendPacket() ret[{}] failed[{}]", ret, m_packet_writer->GetMainName());
    }

    return ret;
}

int SendWorkerSyncQueue::SendPacket(std::function<void(CPacketWriter*)> func, int ms)
{
    func(m_packet_writer.get());

    auto func_producer = [this](std::shared_ptr<QueueSharedMemory> queue) -> bool
    {
        int ret = queue->Push(m_packet_writer->GetBufferPtr(), (uint32_t)m_packet_writer->GetPos());
        return 0 == ret ? true : false;
    };

    int ret = m_send_sync->SyncProducer(func_producer, ms);
    if (ret && INFINITE == ms) {
        LOGE << fmt::format("Send worker SendPacket() ret[{}] failed[{}]", ret, m_packet_writer->GetMainName());
    }

    return ret;
}

int SendWorkerSyncQueue::SendPacket(const std::string& packet_name, int len, std::function<void(void* data)> func, int ms)
{
    m_packet_writer->Start(packet_name);
    m_packet_writer->Write("DATA", len, func);
    m_packet_writer->End();

    auto func_producer = [this](std::shared_ptr<QueueSharedMemory> queue) -> bool
    {
        int ret = queue->Push(m_packet_writer->GetBufferPtr(), (uint32_t)m_packet_writer->GetPos());
        return 0 == ret ? true : false;
    };

    int ret = m_send_sync->SyncProducer(func_producer, ms);
    if (ret && INFINITE == ms) {
        LOGE << fmt::format("Send worker SendPacket() ret[{}] failed[{}]", ret, m_packet_writer->GetMainName());
    }

    return ret;
}



