#include "stdafx.h"
#include "RecvWorkerSyncQueue.h"

#include "common/Util.h"
#include "common/QueueSharedMemory.h"
#include "common/SyncSharedMemory.h"
#include "PacketDefine.h"

//////////////////////////////////////////////////////////////////////////

RecvWorkerSyncQueue::RecvWorkerSyncQueue()
    : m_recv_queue(std::make_shared<QueueSharedMemory>())
    , m_recv_sync(std::make_unique<SyncSharedMemory>())
    , m_packet_reader(std::make_unique<CPacketReader>())
{

}

RecvWorkerSyncQueue::~RecvWorkerSyncQueue()
{

}

int RecvWorkerSyncQueue::Activate(const ActivateParam& param)
{
    // Init QueueSharedMemory
    if (m_recv_queue->Initialize(param.queue_name, 1024 * 1024 * 16))
        return 1;

    // Init SyncSharedMemory
    SyncSharedMemory::ActivateParam sync_init;
    sync_init.producer_count = param.sync_count;
    sync_init.consumer_count = param.sync_count;
    sync_init.queue          = m_recv_queue;
    if (m_recv_sync->Activate(sync_init))
        return 2;

    m_packet_buffer.resize(1024 * 1024 * 3);

    if (param.queue_clear)
        ClearQueue();

    m_callback_reader = param.callback_reader;
    InnerThread::SaveThreadName(param.queue_name);
    InnerThread::StartThread();

    return 0;
}

int RecvWorkerSyncQueue::Deactivate()
{
    LOGI << "Deactivate start...";
    m_thread_running = false;

    m_recv_sync->Deactivate();
    InnerThread::JoinThread();
    m_recv_queue->Finalize();
    LOGI << "Deactivate end...";

    return 0;
}

int RecvWorkerSyncQueue::ClearQueue()
{
    return m_recv_sync->SyncClear();
}

std::string RecvWorkerSyncQueue::GetQueueName() const
{
    return m_recv_queue->GetName();
}

int RecvWorkerSyncQueue::GetSyncCount() const
{
    return m_recv_sync->GetConsumerCount();
}

bool RecvWorkerSyncQueue::CallbackConsumer(std::shared_ptr<QueueSharedMemory>& queue)
{
    int header_len = m_packet_reader->GetSizeSTX() + m_packet_reader->GetSizeMainLength();

    // ���� �޸𸮿� Packet �� ���Դ��� �պκи� ���� �о�ͼ� üũ �Ѵ�.
    if (queue->Front(m_packet_buffer.data(), header_len))
        return false;
    if (false == m_packet_reader->HasSTX(m_packet_buffer.data()))
        return false;

    // ���� �޸𸮿� Packet �� Main Length ���� ����Ǿ� �ִ��� Ȯ�� �Ѵ�.
    uint32_t main_len = m_packet_reader->GetMainLength(m_packet_buffer.data());
    if (main_len > queue->GetUseSize())
        return false;

    // buffer ũ�⸦ �����Ͽ� main_len ���� ���� ������ �Ǵ��� Ȯ���Ѵ�.
    if (m_packet_buffer.size() < main_len)
        m_packet_buffer.resize(main_len);

    // Packet �� ���� �Ѵ�.
    if (queue->Front(m_packet_buffer.data(), main_len))
        return false;

    // Packet �� ���� �Ѵ�.
    if (queue->Pop())
        return false;

    return true;
}

void RecvWorkerSyncQueue::ThreadLoop()
{
    m_thread_running = true;
    while (m_thread_running)
    {
        auto func_consumer = [this](std::shared_ptr<QueueSharedMemory> queue) -> bool {
            if (false == m_thread_running)
                return false;
            return CallbackConsumer(queue);
        };

        int ret = m_recv_sync->SyncConsumer(func_consumer);
        if (SyncSharedMemory::Ret_RecvDeactivaed == ret)
            break;

        if (ret)
        {
            //LOGE << fmt::format("SyncConsumer failed... Name[{}] ret[{}]", m_recv_queue->GetName(), ret);
            continue;
        }

        uint32_t main_len = m_packet_reader->GetMainLength(m_packet_buffer.data());
        // Packet �� �м� �Ѵ�.
        if (m_packet_reader->DoAnalyze(m_packet_buffer.data(), main_len))
        {
            LOGE << "Packet analyze failed : " << m_recv_queue->GetName();
            continue;
        }

        if (m_callback_reader)
            m_callback_reader(m_packet_reader->GetReadMainName(), m_packet_reader.get());
    }

    LOGI << "Recv worker thread finished." << GetQueueName();
}
