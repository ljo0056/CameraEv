#include "stdafx.h"
#include "SyncSharedMemory.h"
#include "QueueSharedMemory.h"

SyncSharedMemory::SyncSharedMemory()
{

}

SyncSharedMemory::~SyncSharedMemory()
{
    Deactivate();
}

int SyncSharedMemory::Activate(const ActivateParam& data)
{
    ActivateParam ap = data;
    if (false == ap.queue->HasQueueInfo())
        return Ret_SharedMemoryInitFailed;

    m_queue = ap.queue;
    if (ReadSharedMemoryInfo(ap))
    {
        // Shared Memory 의 필요한 정보를 다른 사용자가 읽어와서 쓸수 있게 기록 한다.
        WriteSharedMemoryInfo(ap);
    }

    // Main name 은 Queue 의 이름을 사용한다.
    std::string name = ap.queue->GetName();

    char object_name[128] = { 0, };
    snprintf(object_name, 128, ("%s-SamaphoreProducer"), name.c_str());
    m_semaphore_producer = CreateSemaphoreA(NULL, ap.producer_count, ap.producer_count, object_name);
    if (NULL == m_semaphore_producer)
        return Ret_CreateSemaphoreObjectFailed;

    // Consumer Semaphore 의 초기화 값이 0인 이유는 최초에 Queue 생성 시에 가져갈 데이터가 없기 때문이다.
    snprintf(object_name, 128, ("%s-SemaphoreConsumer"), name.c_str());
    m_semaphore_consumer = CreateSemaphoreA(NULL, 0, ap.consumer_count, object_name);
    if (NULL == m_semaphore_consumer)
        return Ret_CreateSemaphoreObjectFailed;

    snprintf(object_name, 128, ("%s-Mutex"), name.c_str());
    m_mutex_queue = CreateMutexA(NULL, FALSE, object_name);
    if (NULL == m_mutex_queue)
        return Ret_CreateMutexObjectFailed;

    m_event_exit = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (NULL == m_event_exit)
        return Ret_CreateEventObjectFailed;

    snprintf(object_name, 128, ("%s-Unlock"), name.c_str());
    m_event_unlock = CreateEventA(NULL, FALSE, FALSE, NULL);
    if (NULL == m_event_unlock)
        return Ret_CreateEventObjectFailed;

    m_producer_count = ap.producer_count;
    m_consumer_count = ap.consumer_count;

    return 0;
}

int SyncSharedMemory::Deactivate()
{
    if (m_event_exit)
    {
        SetEvent(m_event_exit);
        CloseHandle(m_event_exit);
        m_event_exit = NULL;
    }

    if (m_event_unlock)
    {
        CloseHandle(m_event_unlock);
        m_event_unlock = NULL;
    }

    if (m_mutex_queue)
    {
        ReleaseMutex(m_mutex_queue);
        CloseHandle(m_mutex_queue);
        m_mutex_queue = NULL;
    }

    if (m_semaphore_producer)
    {
        CloseHandle(m_semaphore_producer);
        m_semaphore_producer = NULL;
    }

    if (m_semaphore_consumer)
    {
        CloseHandle(m_semaphore_consumer);
        m_semaphore_consumer = NULL;
    }

    return 0;
}

int SyncSharedMemory::ReadSharedMemoryInfo(ActivateParam& param)
{
    uint8_t sm_info[32] = { 0, };
    m_queue->GetInformation(sm_info);

    uint32_t producer_count = 0, consumer_count = 0;

    int pos = 0;
    memcpy(&producer_count, &sm_info[pos], sizeof(producer_count));
    pos += sizeof(producer_count);

    memcpy(&consumer_count, &sm_info[pos], sizeof(consumer_count));
    pos += sizeof(consumer_count);

    if (0 == producer_count)    return Ret_UserInfoReadFailed;
    if (0 == consumer_count)    return Ret_UserInfoReadFailed;

    param.producer_count = producer_count;
    param.consumer_count = consumer_count;

    return 0;
}

int SyncSharedMemory::WriteSharedMemoryInfo(const ActivateParam& param)
{
    uint8_t sm_info[32] = { 0, };

    int pos = 0;
    memcpy(&sm_info[pos], &param.producer_count, sizeof(param.producer_count));
    pos += sizeof(param.producer_count);

    memcpy(&sm_info[pos], &param.consumer_count, sizeof(param.consumer_count));
    pos += sizeof(param.consumer_count);

    m_queue->SetInformation(sm_info);

    return 0;
}

uint32_t SyncSharedMemory::GetProducerCount() const
{
    return m_producer_count;
}

uint32_t SyncSharedMemory::GetConsumerCount() const
{
    return m_consumer_count;
}

void SyncSharedMemory::SetMutexTimeOut(DWORD ms)
{
    m_timeout_mutex = ms;
}

void SyncSharedMemory::SendSyncUnlock()
{
    SetEvent(m_event_unlock);
}

DWORD SyncSharedMemory::WaitForExistSignal(HANDLE handle, DWORD ms)
{
    HANDLE handle_array[3] = { handle, m_event_exit, m_event_unlock };
    return WaitForMultipleObjects(3, handle_array, FALSE, ms);
}

int SyncSharedMemory::SyncProducer(CallBackSync callback, DWORD ms)
{
    FailedCode ret = (FailedCode)0;
    long producer = 0;
    long consumer = 0;

    DWORD semaphore_event = WaitForExistSignal(m_semaphore_producer, ms);
    switch (semaphore_event)
    {
    case WAIT_OBJECT_0:
        {
            DWORD mutex_event = WaitForExistSignal(m_mutex_queue, m_timeout_mutex);
            switch (mutex_event)
            {
            case WAIT_OBJECT_0:
                if (callback(m_queue))
                {
                    if (FALSE == ReleaseSemaphore(m_semaphore_consumer, 1, &consumer))
                    {
                        int nCode = GetLastError();
                        // Semaphore Count 의 최대 개수를 초과해서 값을 증가 할 시에 나오는 에러는 무시
                        if (ERROR_TOO_MANY_POSTS == nCode)
                            ret = (FailedCode)0;
                        else
                            ret = Ret_ReleaseSemaphoreFailed;
                    }
                }
                else
                {
                    // 데이터 추가에 실패하였기 때문에 감소된 Samaphore 값을 원상 복구 시켜줌
                    ReleaseSemaphore(m_semaphore_producer, 1, &producer);
                    ret = Ret_UserFuncFailed;
                }
                ReleaseMutex(m_mutex_queue);
                break;
            case WAIT_OBJECT_0 + 1:
                ret = Ret_RecvDeactivaed;
                break;
            case WAIT_OBJECT_0 + 2:
                ret = Ret_RecvUnlock;
                break;
            case WAIT_TIMEOUT:
                ret = Ret_MutexTimeout;
            default:
                ret = Ret_WaitForObjectFailed;
                break;
            }
        }
        break;
    case WAIT_OBJECT_0 + 1:
        ret = Ret_RecvDeactivaed;
        break;
    case WAIT_OBJECT_0 + 2:
        ret = Ret_RecvUnlock;
        break;
    case WAIT_TIMEOUT:
        ret = Ret_SemaphoreTimeout;
        break;
    default:
        ret = Ret_WaitForObjectFailed;
        break;
    }

    return ret;
}

int SyncSharedMemory::SyncConsumer(CallBackSync callback, DWORD ms)
{
    FailedCode ret = (FailedCode)0;
    long producer = 0;
    long consumer = 0;

    DWORD semaphore_event = WaitForExistSignal(m_semaphore_consumer, ms);
    switch (semaphore_event)
    {
    case WAIT_OBJECT_0:
        {
            DWORD mutex_event = WaitForExistSignal(m_mutex_queue, m_timeout_mutex);
            switch (mutex_event)
            {
            case WAIT_OBJECT_0:
                if (callback(m_queue))
                {
                    if (FALSE == ReleaseSemaphore(m_semaphore_producer, 1, &consumer))
                    {
                        int code = GetLastError();
                        // Semaphore Count 의 최대 개수를 초과해서 값을 증가 할 시에 나오는 에러는 무시
                        if (ERROR_TOO_MANY_POSTS == code)
                            ret = (FailedCode)0;
                        else
                            ret = Ret_ReleaseSemaphoreFailed;
                    }
                }
                else
                {
                    // 데이터 추출에 실패하였기 때문에 감소된 Samaphore 값을 원상 복구 시켜줌
                    ReleaseSemaphore(m_semaphore_consumer, 1, &consumer);
                    ret = Ret_UserFuncFailed;
                }
                ReleaseMutex(m_mutex_queue);
                break;
            case WAIT_OBJECT_0 + 1:
                ret = Ret_RecvDeactivaed;
                break;
            case WAIT_OBJECT_0 + 2:
                ret = Ret_RecvUnlock;
                break;
            case WAIT_TIMEOUT:
                ret = Ret_MutexTimeout;
            default:
                ret = Ret_WaitForObjectFailed;
                break;
            }
        }
        break;
    case WAIT_OBJECT_0 + 1:
        ret = Ret_RecvDeactivaed;
        break;
    case WAIT_OBJECT_0 + 2:
        ret = Ret_RecvUnlock;
        break;
    case WAIT_TIMEOUT:
        ret = Ret_SemaphoreTimeout;
        break;
    default:
        ret = Ret_WaitForObjectFailed;
        break;
    }

    return ret;
}

int SyncSharedMemory::SyncClear(DWORD ms)
{
    FailedCode ret = (FailedCode)0;

    DWORD mutex_event = WaitForExistSignal(m_mutex_queue, m_timeout_mutex);
    switch (mutex_event)
    {
    case WAIT_OBJECT_0:
        m_queue->Clear();
        ResetSemaphore();
        ReleaseMutex(m_mutex_queue);
        break;
    case WAIT_OBJECT_0 + 1:
        ret = Ret_RecvDeactivaed;
        break;
    case WAIT_OBJECT_0 + 2:
        ret = Ret_RecvUnlock;
        break;
    case WAIT_TIMEOUT:
        ret = Ret_MutexTimeout;
    default:
        ret = Ret_WaitForObjectFailed;
        break;
    }

    return ret;
}

int SyncSharedMemory::ResetSemaphore()
{
    long producer = 0;
    long consumer = 0;

    // Queue 가 비었기 때문에 데이터를 삽입 할 수 있는 상태로 만든다.
    // Producer Semaphore Count 를 최대로 증가
    for (uint32_t ii = 0 ; ii < m_producer_count ; ii++)
    {
        if (FALSE == ReleaseSemaphore(m_semaphore_producer, 1, &producer))
            break;
    }

    // Queue 에 가져갈 데이터가 없는 상태로 만든다.
    // Consumer Semaphore Count 는 0으로 변경
    for (uint32_t ii = 0 ; ii < m_consumer_count ; ii++)
    {
        if (WAIT_OBJECT_0 != WaitForSingleObject(m_semaphore_consumer, 0))
            break;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////
// Test code

CTest_SyncDB::CTest_SyncDB()
    : m_writer(new SyncSharedMemory)
    , m_reader(new SyncSharedMemory)
{

}

CTest_SyncDB::~CTest_SyncDB()
{
    m_writer->Deactivate();
    m_reader->Deactivate();
}

int CTest_SyncDB::Test()
{
    // 공유 메모리 객체(QueueSharedMemory) 초기화
    std::string queue_name = "TEST_SYNC_QUEUE";
    const int queue_size = 128;
    std::shared_ptr<QueueSharedMemory> queue_writer = std::make_shared<QueueSharedMemory>();
    std::shared_ptr<QueueSharedMemory> queue_reader = std::make_shared<QueueSharedMemory>();

    if (queue_writer->Initialize(queue_name, queue_size))   return 1;
    if (queue_reader->Initialize(queue_name, queue_size))   return 2;

    // 동기화 공유 메모리 객체(SyncSharedMemory) 초기화
    SyncSharedMemory::ActivateParam sync_init;
    sync_init.producer_count = 1;
    sync_init.consumer_count = 1;
    sync_init.queue = queue_writer;

    if (m_writer->Activate(sync_init))
        return 10;

    sync_init.queue = queue_reader;
    if (m_reader->Activate(sync_init))
        return 11;

    uint8_t  input_data[128]  = "Input Data";
    uint8_t  output_data[128] = { 0, };
    uint32_t input_data_len   = (uint32_t)strlen((char*)input_data);

    //////////////////////////////////////////////////////////////////////////
    // Write
    auto funcInput = [&input_data, input_data_len](std::shared_ptr<QueueSharedMemory> queue) -> bool
    {
        int ret = queue->Push(input_data, input_data_len);
        return 0 == ret ? true : false;
    };

    if (m_writer->SyncProducer(funcInput))
        return 12;

    //////////////////////////////////////////////////////////////////////////
    // Read
    auto funcOutput = [&output_data](std::shared_ptr<QueueSharedMemory> queue) -> bool
    {
        if (queue->Front(output_data, queue->GetUseSize()))
            return false;
        if (queue->Pop())
            return false;
        return true;
    };

    if (m_reader->SyncConsumer(funcOutput))
        return 13;

    //////////////////////////////////////////////////////////////////////////
    // 결과 확인
    if (strcmp((char*)input_data, (char*)output_data))
        return 14;

    return 0;
}

