#include "stdafx.h"
#include "QueueLocalMemory.h"


//////////////////////////////////////////////////////////////////////////

class QueueLocalMemory::QueueLocalMemoryImpl
{
private:
    // Local Memory 상에서 공유 되는 Queue 의 정보
    struct QueueInfo
    {
        uint32_t    tail = 0;
        uint32_t    head = 0;
        uint32_t    queue_size = 0;
        uint32_t    use_size = 0;         // 실제 Queue 에 저장된 Data의 Byte 사이즈
    };

    std::string    m_name;

    QueueInfo      m_queue_info;
    uint8_t*       m_queue_buffer = nullptr;

    uint32_t       m_pop_data_len = 0;

private:
    bool CreateLocalMemory(int queue_size)
    {
        if (0 == queue_size)
            return false;

        m_queue_buffer = new uint8_t[queue_size];
        if (nullptr == m_queue_buffer)
            return false;

        return true;
    }

public:
    QueueLocalMemoryImpl()
        : m_queue_buffer(nullptr)
        , m_pop_data_len(0)
    {

    }

    virtual ~QueueLocalMemoryImpl()
    {
        Finalize();
    }

    int Initialize(const std::string& name, uint32_t queue_size)
    {
        Finalize();

        m_name = name;

        if (false == CreateLocalMemory(queue_size))
            return CREATE_MAMORY;

        m_queue_info.queue_size = queue_size;

        return 0;
    }

    void Finalize()
    {
        if (m_queue_buffer)
        {
            delete[] m_queue_buffer;
            m_queue_buffer = nullptr;
        }
    }

    std::string GetName() const
    {
        return m_name;
    }

    int Clear()
    {
        if (nullptr == m_queue_buffer)
            return DID_NOT_INITIALIZE;

        memset(m_queue_buffer, 0, sizeof(uint8_t) * GetQueueSize());
        m_queue_info.head = 0;
        m_queue_info.tail = 0;
        m_queue_info.use_size = 0;

        return 0;
    }

    int Push(uint8_t* buffer, uint32_t buffer_len)
    {
        if (nullptr == m_queue_buffer)
            return DID_NOT_INITIALIZE;

        if (buffer_len > GetFreeSize())
            return NOT_ENOUGH_FREE_SPACE;

        // --- : 데이터 없음  *** : 데이터 있음
        //   0                                      queue_size
        //   |--------------H************T--------------|
        //                               |----------| <=== write_size
        uint32_t write_size = GetQueueSize() - m_queue_info.tail;
        if (write_size < buffer_len)
        {
            //   0                                      queue_size
            //   |--------------H************T--------------|
            //                               |--------------| <=== write_size Push
            // Tail 뒤에 buffer 를 write_size 크기 만큼 복사 한다.
            memcpy(&m_queue_buffer[m_queue_info.tail], buffer, write_size);

            //   0                                      queue_size
            //   |--------------H************T--------------|
            //   |-----------| <=== (buffer_len - write_size) Push
            // 남은 데이터를 복사 한다.
            memcpy(&m_queue_buffer[0], &buffer[write_size], buffer_len - write_size);
            m_queue_info.tail = buffer_len - write_size;
        }
        else
        {
            //   0                                      queue_size
            //   |--------------H************T--------------|
            //                               |----------|  <== buffer_len Push
            memcpy(&m_queue_buffer[m_queue_info.tail], buffer, buffer_len);
            m_queue_info.tail += buffer_len;
        }

        m_queue_info.use_size += buffer_len;

        return 0;
    }

    int Front(uint8_t* buffer, uint32_t buffer_len)
    {
        if (nullptr == m_queue_buffer)
            return DID_NOT_INITIALIZE;

        if (m_queue_info.use_size < buffer_len)
            return READ_BUFFER_SIZE_IS_BIG;

        // --- : 데이터 없음  *** : 데이터 있음
        //   0                                      queue_size
        //   |**************T------------H**************|
        //                               |----------| <=== read_size
        uint32_t read_size = GetQueueSize() - m_queue_info.head;
        if (read_size < buffer_len)
        {
            //   0                                      queue_size
            //   |**************T------------H**************|
            //                               |--------------| <=== read_size Pop
            memcpy(buffer, &m_queue_buffer[m_queue_info.head], read_size);
            //   0                                      queue_size
            //   |**************T------------H**************|
            //   |--------| <=== (buffer_len - read_size) Pop
            memcpy(&buffer[read_size], &m_queue_buffer[0], buffer_len - read_size);
        }
        else
        {
            //   0                                      queue_size
            //   |--------------H************T--------------|
            //                  |----------| <=== buffer_len Pop
            memcpy(buffer, &m_queue_buffer[m_queue_info.head], buffer_len);
        }

        m_pop_data_len = buffer_len;

        return 0;
    }

    int Pop()
    {
        if (0 == m_pop_data_len)
            return POP_DATA_EMPTY;

        uint32_t nReadSize = GetQueueSize() - m_queue_info.head;
        if (nReadSize < m_pop_data_len)
            m_queue_info.head = m_pop_data_len - nReadSize;
        else
            m_queue_info.head = m_queue_info.head + m_pop_data_len;

        m_queue_info.use_size -= m_pop_data_len;

        m_pop_data_len = 0;

        return 0;
    }

    int SetData(uint32_t pos, uint8_t* buffer, uint32_t buffer_len)
    {
        if (nullptr == m_queue_buffer)
            return DID_NOT_INITIALIZE;
        if (pos < 0 || pos + buffer_len >= GetQueueSize())
            return RANGE_IS_NOT_RIGHT;

        memcpy(&m_queue_buffer[pos], buffer, buffer_len);

        return 0;
    }

    int GetData(uint32_t pos, uint8_t* buffer, uint32_t buffer_len)
    {
        if (nullptr == m_queue_buffer)
            return DID_NOT_INITIALIZE;
        if (pos < 0 || pos >= GetQueueSize())
            return RANGE_IS_NOT_RIGHT;
        if (GetQueueSize() - pos < buffer_len)
            return RANGE_IS_NOT_RIGHT;

        memcpy(buffer, &m_queue_buffer[pos], buffer_len);

        return 0;
    }

    uint32_t GetUseSize() const
    {
        if (nullptr == m_queue_buffer)
            return 0;

        return m_queue_info.use_size;
    }

    uint32_t GetQueueSize() const
    {
        if (nullptr == m_queue_buffer)
            return 0;

        return m_queue_info.queue_size;
    }

    uint32_t GetFreeSize() const
    {
        if (nullptr == m_queue_buffer)
            return 0;

        return (m_queue_info.queue_size - sizeof(uint8_t)) - m_queue_info.use_size;
    }
};

//////////////////////////////////////////////////////////////////////////

QueueLocalMemory::QueueLocalMemory()
    : m_impl(new QueueLocalMemoryImpl)
{

}

QueueLocalMemory::~QueueLocalMemory()
{

}

int QueueLocalMemory::Initialize(const std::string& name, uint32_t queue_size)
{
    return m_impl->Initialize(name, queue_size);
}

void QueueLocalMemory::Finalize()
{
    m_impl->Finalize();
}

std::string QueueLocalMemory::GetName() const
{
    return m_impl->GetName();
}

int QueueLocalMemory::Clear()
{
    return m_impl->Clear();
}

int QueueLocalMemory::Push(uint8_t* buffer, uint32_t buffer_len)
{
    return m_impl->Push(buffer, buffer_len);
}

int QueueLocalMemory::Front(uint8_t* buffer, uint32_t buffer_len)
{
    return m_impl->Front(buffer, buffer_len);
}

int QueueLocalMemory::Pop()
{
    return m_impl->Pop();
}

int QueueLocalMemory::SetData(uint32_t pos, uint8_t* buffer, uint32_t buffer_len)
{
    return m_impl->SetData(pos, buffer, buffer_len);
}

int QueueLocalMemory::GetData(uint32_t pos, uint8_t* buffer, uint32_t buffer_len)
{
    return m_impl->GetData(pos, buffer, buffer_len);
}

uint32_t QueueLocalMemory::GetUseSize() const
{
    return m_impl->GetUseSize();
}

uint32_t QueueLocalMemory::GetQueueSize() const
{
    return m_impl->GetQueueSize();
}

uint32_t QueueLocalMemory::GetFreeSize() const
{
    return m_impl->GetFreeSize();
}


//////////////////////////////////////////////////////////////////////////
// Test Code

int TestQueueLocalMemory()
{
    std::string name = "MyFileMappingObject";
    std::string str_send;
    uint8_t buffer[32] = { 0, };

    // 초기화
    QueueLocalMemory queue;
    if (queue.Initialize(name, 128))
        return 1;

    // Write
    str_send = "hello world";
    memcpy(buffer, str_send.c_str(), str_send.size());
    queue.Push(buffer, (uint32_t)str_send.size());

    // Read
    std::string str_recv;
    queue.Front(buffer, 11);
    queue.Pop();
    str_recv = (char*)buffer;

    // 검증
    if (str_send != str_recv)
        return 3;

    return 0;
}
