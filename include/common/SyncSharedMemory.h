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
///  @brief   QueueSharedMemory �� �̿��Ͽ� ���μ����� ����� �����ϵ��� ����ȭ�� ���ش�.
///           �����ڿ� �Һ��� ������� �̿��Ͽ� �����͸� ����, �����͸� �Һ��ϴ� ���μ����� �и��Ͽ� ����ȭ �Ѵ�.

class SyncSharedMemory final
{
public:
    struct ActivateParam
    {
        uint32_t     producer_count = 0;    // ������ �������� ī��Ʈ�� ����
        uint32_t     consumer_count = 0;    // �Һ��� �������� ī��Ʈ�� ����

        std::shared_ptr<QueueSharedMemory> queue;  // ���� �޸�
    };

    enum FailedCode
    {
        Ret_SharedMemoryInitFailed = 1,     // ���� �޸� �ʱ�ȭ�� �Ǿ� ���� ����
        Ret_CreateSemaphoreObjectFailed,    // �������� ��ü ���� ����
        Ret_CreateMutexObjectFailed,        // ���ؽ� ��ü ���� ����
        Ret_CreateEventObjectFailed,        // �̺�Ʈ ��ü ���� ����
        Ret_UserInfoReadFailed,             // ���� �޸� User Info �������� �޸� �б� ����
        Ret_RecvDeactivaed,                 // Deactivate() �Լ� ȣ���� �Ǿ���
        Ret_RecvUnlock,                     // Block ���¸� Ǫ�� �Լ� ȣ���� �Ǿ���
        Ret_SemaphoreTimeout,               // �������� ��� �ð� Ÿ�Ӿƿ�
        Ret_MutexTimeout,                   // ���ؽ� ��� �ð� Ÿ�Ӿƿ�
        Ret_WaitForObjectFailed,            // WaitForMultipleObjects() ��� ���� ����
        Ret_UserFuncFailed,                 // CallBackSync �ݹ� �Լ��� ���ϰ��� false ��
        Ret_ClearFailed,                    // ���� �޸��� ���� queue clear ����
        Ret_ReleaseSemaphoreFailed,         // ReleaseSemaphore() ��� ����
        Ret_Num,
    };

    // ����, �Һ� �� �� �ִٸ� (�������� ī��Ʈ üũ) ȣ��Ǵ� �ݹ� �Լ�
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

    ///  @brief      Ȱ��ȭ, ��Ȱ��ȭ ��Ų��.
    ///  @return     ���� �ÿ� 0, ���� �ÿ� FailedCode ���� ����
    int  Activate(const ActivateParam& data);
    int  Deactivate();

    ///  @brief : ������ �������� ī��Ʈ�� ���� �Ѵ�.
    ///  @return :  ������ �������� ī��Ʈ
    uint32_t GetProducerCount() const;
    ///  @brief : �Һ��� �������� ī��Ʈ�� ���� �Ѵ�.
    ///  @return :  �Һ��� �������� ī��Ʈ
    uint32_t GetConsumerCount() const;

    ///  @brief : QueueSharedMemory �� �Ӱ迵���� �����ϴ� ���ؽ��� Ÿ�� �ƿ� ����
    ///  @param ms[in] : Ÿ�Ӿƿ� �ð� ���� (millisecond)
    ///  @return : ����
    void SetMutexTimeOut(DWORD ms);

    ///  @brief : SyncProducer(), SyncConsumer(), SyncClear() �Լ����� Block �� �ɷ��ִ� ���¸� Ǯ���ش�.
    ///  @return :  ����
    void SendSyncUnlock();

    ///  @brief : ������, �Һ����� �������� ī��Ʈ�� �ʱ�ȭ �����ش�.
    ///  @return :  ���� �ÿ� 0, ���� �ÿ� FailedCode ���� ����
    int  ResetSemaphore();

    ///  @brief : QueueSharedMemory �� �����͸� Push �����ϴٸ� callback �Լ��� ȣ�� ���ְ�
    ///           ������ ������� ���ҽ�Ű�� �Һ��� ������� �������� SyncConsumer() ���� �����͸� Pop �����ϵ��� �Ѵ�.
    ///  @param callback[in] : �����Ͱ� Push ������ �ÿ� ȣ��Ǵ� callback �Լ�
    ///  @param ms[in] : Ÿ�Ӿƿ� �ð� ���� (millisecond)
    ///  @return : ���� �ÿ� 0, ���� �ÿ� FailedCode ���� ����
    int  SyncProducer(CallBackSync callback, DWORD ms = INFINITE);

    ///  @brief : QueueSharedMemory �� �����͸� Pop �����ϴٸ� callback �Լ��� ȣ�� ���ְ�
    ///           �Һ��� ������� ���ҽ�Ű�� ������ ������� �������� SyncProducer() ���� �����͸� Push �����ϵ��� �Ѵ�.
    ///  @param callback[in] : �����Ͱ� Pop ������ �ÿ� ȣ��Ǵ� callback �Լ�
    ///  @param ms[in] : Ÿ�Ӿƿ� �ð� ���� (millisecond)
    ///  @return : ���� �ÿ� 0, ���� �ÿ� FailedCode ���� ����
    int  SyncConsumer(CallBackSync callback, DWORD ms = INFINITE);

    ///  @brief : QueueSharedMemory �� �ִ� ��� ����� �����͸� ��� �����ϰ� ResetSemaphore()�� ȣ�����ش�.
    ///  @param ms[in] : Ÿ�Ӿƿ� �ð� ���� (millisecond)
    ///  @return : ���� �ÿ� 0, ���� �ÿ� FailedCode ���� ����
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
