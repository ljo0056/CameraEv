#pragma once

//////////////////////////////////////////////////////////////////////////
///  @file    QueueLocalMemory.h
///  @date    2019/06/13
///  @author  Lee Jong Oh
///

//////////////////////////////////////////////////////////////////////////
///  @class   QueueLocalMemory
///  @brief   원형 Queue 를 생성 한다.

#include <memory>
#include <string>

class QueueLocalMemory
{
private:
    class QueueLocalMemoryImpl;
    std::unique_ptr<QueueLocalMemoryImpl> m_impl;

public:
    enum FailedCode
    {
        CREATE_MAMORY = 1,              // 메모리 생성에 실패
        DID_NOT_INITIALIZE,             // 초기화를 수행하지 않았음
        NOT_ENOUGH_FREE_SPACE,          // Queue 의 여유 공간이 적음
        READ_BUFFER_SIZE_IS_BIG,        // Read 하고자 하는 buffer 사이즈가 Queue 에 적재된 Use size 보다 큼
        POP_DATA_EMPTY,                 // Pop 할 데이터가 없음
        RANGE_IS_NOT_RIGHT,             // 범위가 맞지 않음
    };

    QueueLocalMemory();
    ~QueueLocalMemory();

    ///  @brief      Local Memory 사용을 위해 객체를 할당하고 초기화 시킨다.
    ///  @param name[in] : Local Memory 에 이름을 설정
    ///  @param queue_size[in] : Byte 단위의 Queue size 를 설정
    ///  @return     성공 시에 0, 실패 시에 FailedCode 을 return 한다.
    int  Initialize(const std::string& name, uint32_t queue_size);

    ///  @brief      Local Memory 사용을 위해 생성된 객체들을 정리 한다.
    void Finalize();

    ///  @brief      Initialize 함수의 name 파라미터를 return 한다.
    ///  @return     성공 시에 Local Memory 의 name 을 return, 실패시에 빈 문자열 return
    std::string GetName() const;

    ///  @brief      Local Memory 의 Queue 를 비운다.
    ///  @return     성공 시에 0, 실패 시에 FailedCode 을 return 한다.
    int Clear();

    ///  @brief      Local Memory 의 Queue 에 데이터를 뒤에 추가 한다.
    ///  @param buffer[in] : buffer 의 데이터를 buffer_len 길이 만큼 Queue 에 copy 한다.
    ///  @param buffer_len[in] : buffer 의 데이터 길이 (Byte)
    ///  @return     성공 시에 0, 실패 시에 FailedCode 을 return 한다.
    int Push(uint8_t* buffer, uint32_t buffer_len);

    ///  @brief      Local Memory 의 Queue 에서 제일 앞의 데이터를 가져온다.
    ///              가져 온후에 Pop() 함수를 호출해야 Queue 에서 데이터가 삭제 된다.
    ///  @param buffer[out] : buffer 의 데이터를 buffer_len 길이 만큼 Queue 에서 copy 한다.
    ///  @param buffer_len[in] : buffer 의 데이터 길이 (Byte)
    ///  @return     성공 시에 0, 실패 시에 FailedCode 을 return 한다.
    int Front(uint8_t* buffer, uint32_t buffer_len);

    ///  @brief      Front() 함수 호출 후에 Queue 에서 데이터를 삭제 한다.    
    ///  @return     성공 시에 0, 실패 시에 FailedCode 을 return 한다.
    int Pop();

    ///  @brief      Local Memory Queue 에 특정한 위치에 데이터를 copy 한다.
    ///  @param pos[in] : Queue 의 위치
    ///  @param buffer[in] : buffer 의 데이터를 buffer_len 길이 만큼 Queue 에 copy 한다.
    ///  @param buffer_len[in] : buffer 의 데이터 길이 (Byte)
    ///  @return     성공 시에 0, 실패 시에 FailedCode 을 return 한다.
    int SetData(uint32_t pos, uint8_t* buffer, uint32_t buffer_len);

    ///  @brief      Local Memory Queue 에 특정한 위치에 데이터를 buffer 에 copy 한다.
    ///  @param pos[in] : Queue 의 위치
    ///  @param buffer[out] : buffer 의 데이터를 buffer_len 길이 만큼 Queue 에서 copy 한다.
    ///  @param buffer_len[in] : buffer 의 데이터 길이 (Byte)
    ///  @return     성공 시에 0, 실패 시에 FailedCode 을 return 한다.
    int GetData(uint32_t pos, uint8_t* buffer, uint32_t buffer_len);

    ///  @brief      Local Memory Queue 에 적제된 데이터 Byte 크기를 return 한다.
    ///  @return     성공 시에 Queue 에 사용된 Byte 크기, 실패 시에 0 을 return 한다.
    uint32_t GetUseSize() const;

    ///  @brief      Local Memory Queue 의 실제 크기를 return 한다.
    ///  @return     성공 시에 Queue 의 Byte 크기, 실패 시에 0 을 return 한다.
    uint32_t GetQueueSize() const;

    ///  @brief      Local Memory Queue 에 여분의 공간을 Byte 단위 크기로 return 한다.
    ///  @return     성공 시에 Queue 의 여분 Byte 크기, 실패 시에 0 을 return 한다.
    uint32_t GetFreeSize() const;
};


//////////////////////////////////////////////////////////////////////////
// Test Code

int TestQueueLocalMemory();