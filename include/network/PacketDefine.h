#pragma once

///    
///  @file    PacketDefine.h
///  @date    2018/03/06
///  @author  Lee Jong Oh
///
///  history
///  2018/03/06 - 제작
///  2019/05/24 - 코드 스타일 변경 및 변수 타입 변경
///  2019/08/28 - class CPacketWriter 의 buffer 타입을 [uint8_t*] -> [std::vector<uint8_t>] 변경

#include <string>
#include <vector>
#include <functional>

///  @class   CPacketDefine
///  @brief
///
/// Main Length : STX ~ ETX 까지의 Packet 전체 길이
/// Packet 각 Section 의 크기
/// |--- 4byte ---|---    4byte    ---|---Packet Name(String)---|---      1 Byte       ---|---Block Item N 개---|--- 4 Byte ---|
/// Packet 구조
/// |---  STX  ---|--- Main Length ---|---    Packet Name    ---|---StartBlockDelimiter---|---Block Item N 개---|---  ETX   ---|
/// 
/// Block Length : Block 전체 길이
/// Block 각 Section 의 크기
/// |---GetSizeBlockLength---|---Block Name(String)---|---      1 Byte      ---|---Block Data---|
/// Block 구조
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
        std::string name;    // Block 이름
        uint8_t*    data;    // Block 에서 data 의 Pointer 위치
        int         length;  // data 의 길이
    };

    CPacketDefine();
    ~CPacketDefine();

    uint8_t GetSTX(int pos) const;
    uint8_t GetETX(int pos) const;

    // 각 Packet Section 의 시작 위치 반환
    int  GetPosSTX() const;
    int  GetPosETX(int main_length) const;
    int  GetPosMainLength() const;
    int  GetPosMainName() const;

    // 각 Packet Section 의 할당된 크기 반환
    int  GetSizeSTX() const;
    int  GetSizeETX() const;
    int  GetSizeMainLength() const;
    int  GetSizeBlockLength() const;

    // Main Name 과 처음 Block Item 사이의 구분자
    uint8_t GetStartBlockDelimiter() const;
    // Item Block 단위에서 이름과 데이터 사이의 구분자
    uint8_t GetBlockDataDelimiter() const;

    bool HasSTX(uint8_t* packet) const;
    bool HasETX(uint8_t* packet, int main_length) const;

    // Packet 의 Main Length 를 읽어온다.
    uint32_t GetMainLength(uint8_t* packet) const;
    // Packet 의 Main Main 을 읽어온다.
    std::string GetMainName(uint8_t* packet) const;
    // Block 의 Length 를 읽어온다.
    uint32_t GetBlockLength(uint8_t* block) const;
    // Block Item 을 읽어온다.
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

    // Return 값 : Write Position 을 Return 한다.
    int  Start(const std::string& name);
    int  Write(const std::string& name, int32_t data);
    int  Write(const std::string& name, uint32_t data);
    int  Write(const std::string& name, int64_t data);
    int  Write(const std::string& name, uint64_t data);
    int  Write(const std::string& name, long data);
    int  Write(const std::string& name, bool data);
    int  Write(const std::string& name, float data);     // 소수점 5자리 까지
    int  Write(const std::string& name, double data);    // 소수점 5자리 까지
    int  Write(const std::string& name, uint8_t* data, int len);
    int  Write(const std::string& name, const std::string& data);

    // packet 에 Write 할 buffer를 func 함수를 통해서 전달하여 직접 메모리에 쓰도록 하는 함수
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

    // return 성공 name 에 해당하는 BlockItem 의 Length, 실패 0
    int  ReadLength(const std::string& name) const;

    // return 성공 0, 실패 1
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

    // data 에 BlockItem 의 실제 데이터의 포인터 주소만 복사 해준다.
    int  Read(const std::string& name, uint8_t*& data) const;

    // data 에 BlockItem 실제 데이터를 memcpy 로 복사 한다.
    // GetDataLength 함수를 이용하여 buffer 의 길이를 알아와야 메모리 복사가 안정적으로 이루어 진다.
    int  ReadMemCopy(const std::string& name, uint8_t* data) const;

    // data 에 BlockItem 실제 데이터를 memcpy 로 복사 한다.
    int  ReadMemCopy(const std::string& name, std::vector<uint8_t>& data) const;
};

//////////////////////////////////////////////////////////////////////////

// Test Code
int Test_Packet();