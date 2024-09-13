#pragma once

///    
///  @file    PacketDefine.h
///  @date    2018/03/06
///  @author  Lee Jong Oh
///
///  history
///  2018/03/06 - ����
///  2019/05/24 - �ڵ� ��Ÿ�� ���� �� ���� Ÿ�� ����
///  2019/08/28 - class CPacketWriter �� buffer Ÿ���� [uint8_t*] -> [std::vector<uint8_t>] ����

#include <string>
#include <vector>
#include <functional>

///  @class   CPacketDefine
///  @brief
///
/// Main Length : STX ~ ETX ������ Packet ��ü ����
/// Packet �� Section �� ũ��
/// |--- 4byte ---|---    4byte    ---|---Packet Name(String)---|---      1 Byte       ---|---Block Item N ��---|--- 4 Byte ---|
/// Packet ����
/// |---  STX  ---|--- Main Length ---|---    Packet Name    ---|---StartBlockDelimiter---|---Block Item N ��---|---  ETX   ---|
/// 
/// Block Length : Block ��ü ����
/// Block �� Section �� ũ��
/// |---GetSizeBlockLength---|---Block Name(String)---|---      1 Byte      ---|---Block Data---|
/// Block ����
/// |---   Block Length   ---|---    Block Name    ---|---BlockDataDelimiter---|---Block Data---|
///

class CPacketDefine
{
private:
    static const uint8_t m_stx[4];
    static const uint8_t m_etx[4];

public:
    struct BlockItem
    {
        std::string name;    // Block �̸�
        uint8_t*    data;    // Block ���� data �� Pointer ��ġ
        int         length;  // data �� ����
    };

    CPacketDefine();
    ~CPacketDefine();

    uint8_t GetSTX(int pos) const;
    uint8_t GetETX(int pos) const;

    // �� Packet Section �� ���� ��ġ ��ȯ
    int  GetPosSTX() const;
    int  GetPosETX(int main_length) const;
    int  GetPosMainLength() const;
    int  GetPosMainName() const;

    // �� Packet Section �� �Ҵ�� ũ�� ��ȯ
    int  GetSizeSTX() const;
    int  GetSizeETX() const;
    int  GetSizeMainLength() const;
    int  GetSizeBlockLength() const;

    // Main Name �� ó�� Block Item ������ ������
    uint8_t GetStartBlockDelimiter() const;
    // Item Block �������� �̸��� ������ ������ ������
    uint8_t GetBlockDataDelimiter() const;

    bool HasSTX(uint8_t* packet) const;
    bool HasETX(uint8_t* packet, int main_length) const;

    // Packet �� Main Length �� �о�´�.
    uint32_t GetMainLength(uint8_t* packet) const;
    // Packet �� Main Main �� �о�´�.
    std::string GetMainName(uint8_t* packet) const;
    // Block �� Length �� �о�´�.
    uint32_t GetBlockLength(uint8_t* block) const;
    // Block Item �� �о�´�.
    int GetBlock(uint8_t* block, BlockItem& item) const;
};

//////////////////////////////////////////////////////////////////////////

///  @class   CPacketWriter

class CPacketWriter : public CPacketDefine
{
private:
    size_t                  m_pos;
    std::vector<uint8_t>*   m_buffer;

private:
    size_t GetBlockHeaderLength(const std::string& name);
    size_t WriteBlockName(const std::string& name);

    int  CheckBuffer(size_t len);
    void WriteCurBuffer(void const* src, size_t len);
    void WriteBuffer(int pos, void const* src, size_t len);    
    void BufferPushBack(uint8_t data);

public:
    CPacketWriter();
    CPacketWriter(std::vector<uint8_t>* buffer);
    CPacketWriter(std::vector<uint8_t>* buffer, size_t size);
    ~CPacketWriter();

    void SetBuffer(std::vector<uint8_t>* buffer);
    std::vector<uint8_t>* GetBuffer() const;
    uint8_t* GetBufferPtr() const;
    uint8_t* GetCurBufferPtr() const;

    size_t GetBufferLength() const;
    size_t GetPos() const;
    void   Clear();

    std::string GetMainName() const;

    // Return �� : Write Position �� Return �Ѵ�.
    int  Start(const std::string& name);
    int  Write(const std::string& name, int32_t data);
    int  Write(const std::string& name, uint32_t data);
    int  Write(const std::string& name, int64_t data);
    int  Write(const std::string& name, uint64_t data);
    int  Write(const std::string& name, long data);
    int  Write(const std::string& name, bool data);
    int  Write(const std::string& name, float data);     // �Ҽ��� 5�ڸ� ����
    int  Write(const std::string& name, double data);    // �Ҽ��� 5�ڸ� ����
    int  Write(const std::string& name, uint8_t* data, int len);
    int  Write(const std::string& name, const std::string& data);

    // packet �� Write �� buffer�� func �Լ��� ���ؼ� �����Ͽ� ���� �޸𸮿� ������ �ϴ� �Լ�
    int  Write(const std::string& name, int len, std::function<void(void* data)> func);
    int  End();
};

//////////////////////////////////////////////////////////////////////////

///  @class   CPacketReader

class CPacketReader : public CPacketDefine
{
private:
    std::string             m_main_name;
    std::vector<BlockItem>  m_block_items;

public:
    CPacketReader();
    ~CPacketReader();

    int  DoAnalyze(uint8_t* packet, int len);
    void Clear();

    std::string GetReadMainName() const;
    bool HasReadMainName() const;

    // return ���� name �� �ش��ϴ� BlockItem �� Length, ���� 0
    int  ReadLength(const std::string& name) const;

    // return ���� 0, ���� 1
    int  Read(const std::string& name, BlockItem& data) const;
    int  Read(const std::string& name, int32_t& data) const;
    int  Read(const std::string& name, uint32_t& data) const;
    int  Read(const std::string& name, int64_t& data) const;
    int  Read(const std::string& name, uint64_t& data) const;
    int  Read(const std::string& name, long& data) const;
    int  Read(const std::string& name, bool& data) const;
    int  Read(const std::string& name, float& data) const;
    int  Read(const std::string& name, double& data) const;
    int  Read(const std::string& name, std::string& data) const;

    // data �� BlockItem �� ���� �������� ������ �ּҸ� ���� ���ش�.
    int  Read(const std::string& name, uint8_t*& data) const;

    // data �� BlockItem ���� �����͸� memcpy �� ���� �Ѵ�.
    // GetDataLength �Լ��� �̿��Ͽ� buffer �� ���̸� �˾ƿ;� �޸� ���簡 ���������� �̷�� ����.
    int  ReadMemCopy(const std::string& name, uint8_t* data) const;

    // data �� BlockItem ���� �����͸� memcpy �� ���� �Ѵ�.
    int  ReadMemCopy(const std::string& name, std::vector<uint8_t>& data) const;
};

//////////////////////////////////////////////////////////////////////////

// Test Code
int Test_Packet();