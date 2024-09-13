#pragma once

//////////////////////////////////////////////////////////////////////////
///  @file    QueueLocalMemory.h
///  @date    2019/06/13
///  @author  Lee Jong Oh
///

//////////////////////////////////////////////////////////////////////////
///  @class   QueueLocalMemory
///  @brief   ���� Queue �� ���� �Ѵ�.

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
        CREATE_MAMORY = 1,              // �޸� ������ ����
        DID_NOT_INITIALIZE,             // �ʱ�ȭ�� �������� �ʾ���
        NOT_ENOUGH_FREE_SPACE,          // Queue �� ���� ������ ����
        READ_BUFFER_SIZE_IS_BIG,        // Read �ϰ��� �ϴ� buffer ����� Queue �� ����� Use size ���� ŭ
        POP_DATA_EMPTY,                 // Pop �� �����Ͱ� ����
        RANGE_IS_NOT_RIGHT,             // ������ ���� ����
    };

    QueueLocalMemory();
    ~QueueLocalMemory();

    ///  @brief      Local Memory ����� ���� ��ü�� �Ҵ��ϰ� �ʱ�ȭ ��Ų��.
    ///  @param name[in] : Local Memory �� �̸��� ����
    ///  @param queue_size[in] : Byte ������ Queue size �� ����
    ///  @return     ���� �ÿ� 0, ���� �ÿ� FailedCode �� return �Ѵ�.
    int  Initialize(const std::string& name, uint32_t queue_size);

    ///  @brief      Local Memory ����� ���� ������ ��ü���� ���� �Ѵ�.
    void Finalize();

    ///  @brief      Initialize �Լ��� name �Ķ���͸� return �Ѵ�.
    ///  @return     ���� �ÿ� Local Memory �� name �� return, ���нÿ� �� ���ڿ� return
    std::string GetName() const;

    ///  @brief      Local Memory �� Queue �� ����.
    ///  @return     ���� �ÿ� 0, ���� �ÿ� FailedCode �� return �Ѵ�.
    int Clear();

    ///  @brief      Local Memory �� Queue �� �����͸� �ڿ� �߰� �Ѵ�.
    ///  @param buffer[in] : buffer �� �����͸� buffer_len ���� ��ŭ Queue �� copy �Ѵ�.
    ///  @param buffer_len[in] : buffer �� ������ ���� (Byte)
    ///  @return     ���� �ÿ� 0, ���� �ÿ� FailedCode �� return �Ѵ�.
    int Push(uint8_t* buffer, uint32_t buffer_len);

    ///  @brief      Local Memory �� Queue ���� ���� ���� �����͸� �����´�.
    ///              ���� ���Ŀ� Pop() �Լ��� ȣ���ؾ� Queue ���� �����Ͱ� ���� �ȴ�.
    ///  @param buffer[out] : buffer �� �����͸� buffer_len ���� ��ŭ Queue ���� copy �Ѵ�.
    ///  @param buffer_len[in] : buffer �� ������ ���� (Byte)
    ///  @return     ���� �ÿ� 0, ���� �ÿ� FailedCode �� return �Ѵ�.
    int Front(uint8_t* buffer, uint32_t buffer_len);

    ///  @brief      Front() �Լ� ȣ�� �Ŀ� Queue ���� �����͸� ���� �Ѵ�.    
    ///  @return     ���� �ÿ� 0, ���� �ÿ� FailedCode �� return �Ѵ�.
    int Pop();

    ///  @brief      Local Memory Queue �� Ư���� ��ġ�� �����͸� copy �Ѵ�.
    ///  @param pos[in] : Queue �� ��ġ
    ///  @param buffer[in] : buffer �� �����͸� buffer_len ���� ��ŭ Queue �� copy �Ѵ�.
    ///  @param buffer_len[in] : buffer �� ������ ���� (Byte)
    ///  @return     ���� �ÿ� 0, ���� �ÿ� FailedCode �� return �Ѵ�.
    int SetData(uint32_t pos, uint8_t* buffer, uint32_t buffer_len);

    ///  @brief      Local Memory Queue �� Ư���� ��ġ�� �����͸� buffer �� copy �Ѵ�.
    ///  @param pos[in] : Queue �� ��ġ
    ///  @param buffer[out] : buffer �� �����͸� buffer_len ���� ��ŭ Queue ���� copy �Ѵ�.
    ///  @param buffer_len[in] : buffer �� ������ ���� (Byte)
    ///  @return     ���� �ÿ� 0, ���� �ÿ� FailedCode �� return �Ѵ�.
    int GetData(uint32_t pos, uint8_t* buffer, uint32_t buffer_len);

    ///  @brief      Local Memory Queue �� ������ ������ Byte ũ�⸦ return �Ѵ�.
    ///  @return     ���� �ÿ� Queue �� ���� Byte ũ��, ���� �ÿ� 0 �� return �Ѵ�.
    uint32_t GetUseSize() const;

    ///  @brief      Local Memory Queue �� ���� ũ�⸦ return �Ѵ�.
    ///  @return     ���� �ÿ� Queue �� Byte ũ��, ���� �ÿ� 0 �� return �Ѵ�.
    uint32_t GetQueueSize() const;

    ///  @brief      Local Memory Queue �� ������ ������ Byte ���� ũ��� return �Ѵ�.
    ///  @return     ���� �ÿ� Queue �� ���� Byte ũ��, ���� �ÿ� 0 �� return �Ѵ�.
    uint32_t GetFreeSize() const;
};


//////////////////////////////////////////////////////////////////////////
// Test Code

int TestQueueLocalMemory();