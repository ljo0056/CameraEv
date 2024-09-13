#pragma once

//////////////////////////////////////////////////////////////////////////
///  @file    SyncSharedMemory.h
///  @date    2019/08/22
///  @author  Lee Jong Oh


#include <functional>
#include <memory>

class QueueSharedMemory;

//////////////////////////////////////////////////////////////////////////
///  @class   SyncSharedMemory
///  @brief   QueueSharedMemory 를 이용하여 프로세스간 통신이 가능하도록 동기화를 해준다.
///           생상자와 소비자 세마포어를 이용하여 데이터를 생산, 데이터를 소비하는 프로세스로 분리하여 동기화 한다.

class SyncSharedMemory final
{
public:
    struct ActivateParam
    {
        uint32_t     producer_count = 0;    // 생산자 세마포어 카운트를 설정
        uint32_t     consumer_count = 0;    // 소비자 세마포어 카운트를 설정

        std::shared_ptr<QueueSharedMemory> queue;  // 공유 메모리
    };

    enum FailedCode
    {
        Ret_SharedMemoryInitFailed = 1,     // 공유 메모리 초기화가 되어 있지 않음
        Ret_CreateSemaphoreObjectFailed,    // 세마포어 객체 생성 실패
        Ret_CreateMutexObjectFailed,        // 뮤텍스 객체 생성 실패
        Ret_CreateEventObjectFailed,        // 이벤트 객체 생성 실패
        Ret_UserInfoReadFailed,             // 공유 메모리 User Info 공간에서 메모리 읽기 실패
        Ret_RecvDeactivaed,                 // Deactivate() 함수 호출이 되었음
        Ret_RecvUnlock,                     // Block 상태를 푸는 함수 호출이 되었음
        Ret_SemaphoreTimeout,               // 세마포어 대기 시간 타임아웃
        Ret_MutexTimeout,                   // 뮤텍스 대기 시간 타임아웃
        Ret_WaitForObjectFailed,            // WaitForMultipleObjects() 결과 에러 리턴
        Ret_UserFuncFailed,                 // CallBackSync 콜백 함수의 리턴값이 false 임
        Ret_ClearFailed,                    // 공유 메모리의 내부 queue clear 실패
        Ret_ReleaseSemaphoreFailed,         // ReleaseSemaphore() 결과 실패
        Ret_Num,
    };

    // 생산, 소비를 할 수 있다면 (세마포어 카운트 체크) 호출되는 콜백 함수
    using CallBackSync = std::function<bool(std::shared_ptr<QueueSharedMemory> queue)>;

private:
    HANDLE   m_event_exit = NULL;
    HANDLE   m_event_unlock = NULL;
    HANDLE   m_semaphore_producer = NULL;
    HANDLE   m_semaphore_consumer = NULL;
    HANDLE   m_mutex_queue = NULL;

    uint32_t m_producer_count = 0;
    uint32_t m_consumer_count = 0;
    DWORD    m_timeout_mutex  = INFINITE;

    std::shared_ptr<QueueSharedMemory> m_queue;

private:
    DWORD WaitForExistSignal(HANDLE handle, DWORD ms);
    int   WriteSharedMemoryInfo(const ActivateParam& get_data);
    int   ReadSharedMemoryInfo(ActivateParam& get_data);

public:
    SyncSharedMemory();
    ~SyncSharedMemory();

    ///  @brief      활성화, 비활성화 시킨다.
    ///  @return     성공 시에 0, 실패 시에 FailedCode 값을 리턴
    int  Activate(const ActivateParam& data);
    int  Deactivate();

    ///  @brief : 생산자 세마포어 카운트를 리턴 한다.
    ///  @return :  생산자 세마포어 카운트
    uint32_t GetProducerCount() const;
    ///  @brief : 소비자 세마포어 카운트를 리턴 한다.
    ///  @return :  소비자 세마포어 카운트
    uint32_t GetConsumerCount() const;

    ///  @brief : QueueSharedMemory 를 임계영역을 설정하는 뮤텍스의 타임 아웃 설정
    ///  @param ms[in] : 타임아웃 시간 설정 (millisecond)
    ///  @return : 없음
    void SetMutexTimeOut(DWORD ms);

    ///  @brief : SyncProducer(), SyncConsumer(), SyncClear() 함수에서 Block 이 걸려있는 상태를 풀어준다.
    ///  @return :  없음
    void SendSyncUnlock();

    ///  @brief : 생산자, 소비자의 세마포어 카운트를 초기화 시켜준다.
    ///  @return :  성공 시에 0, 실패 시에 FailedCode 값을 리턴
    int  ResetSemaphore();

    ///  @brief : QueueSharedMemory 에 데이터를 Push 가능하다면 callback 함수를 호출 해주고
    ///           생산자 세마포어를 감소시키고 소비자 세마포어를 증가시켜 SyncConsumer() 에서 데이터를 Pop 가능하도록 한다.
    ///  @param callback[in] : 데이터가 Push 가능할 시에 호출되는 callback 함수
    ///  @param ms[in] : 타임아웃 시간 설정 (millisecond)
    ///  @return : 성공 시에 0, 실패 시에 FailedCode 값을 리턴
    int  SyncProducer(CallBackSync callback, DWORD ms = INFINITE);

    ///  @brief : QueueSharedMemory 에 데이터를 Pop 가능하다면 callback 함수를 호출 해주고
    ///           소비자 세마포어를 감소시키고 생산자 세마포어를 증가시켜 SyncProducer() 에서 데이터를 Push 가능하도록 한다.
    ///  @param callback[in] : 데이터가 Pop 가능할 시에 호출되는 callback 함수
    ///  @param ms[in] : 타임아웃 시간 설정 (millisecond)
    ///  @return : 성공 시에 0, 실패 시에 FailedCode 값을 리턴
    int  SyncConsumer(CallBackSync callback, DWORD ms = INFINITE);

    ///  @brief : QueueSharedMemory 에 있는 모든 적재된 데이터를 모두 삭제하고 ResetSemaphore()를 호출해준다.
    ///  @param ms[in] : 타임아웃 시간 설정 (millisecond)
    ///  @return : 성공 시에 0, 실패 시에 FailedCode 값을 리턴
    int  SyncClear(DWORD ms = INFINITE);
};

//////////////////////////////////////////////////////////////////////////
// Test Code

class CTest_SyncDB
{
private:
    std::unique_ptr<SyncSharedMemory>  m_writer;
    std::unique_ptr<SyncSharedMemory>  m_reader;

public:
    CTest_SyncDB();
    ~CTest_SyncDB();

    int Test();
};
