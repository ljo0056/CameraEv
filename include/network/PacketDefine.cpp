#include "stdafx.h"
#include "PacketDefine.h"
#include <string>
#include <iomanip>
#include <sstream>

// 컴파일 warning 메세지 숨김
// size_t 타입 형변환시에 생기는 warning 메세지
#pragma warning (push)
#pragma warning (disable:4267) 

//////////////////////////////////////////////////////////////////////////

const uint8_t CPacketDefine::m_stx[4] = { 0x10, 0x20, 0x30, 0x40 };
const uint8_t CPacketDefine::m_etx[4] = { 0x40, 0x30, 0x20, 0x10 };

CPacketDefine::CPacketDefine()
{

}

CPacketDefine::~CPacketDefine()
{

}

uint8_t CPacketDefine::GetSTX(int pos) const
{
    return m_stx[pos];
}

uint8_t CPacketDefine::GetETX(int pos) const
{
    return m_etx[pos];
}

int CPacketDefine::GetPosSTX() const
{
    return 0;
}

int CPacketDefine::GetPosETX(int main_length) const
{
    return main_length - GetSizeETX();
}

int CPacketDefine::GetPosMainLength() const
{
    return GetPosSTX() + GetSizeSTX();
}

int CPacketDefine::GetPosMainName() const
{
    return GetPosMainLength() + GetSizeMainLength();
}

int CPacketDefine::GetSizeSTX() const
{
    return sizeof(m_stx) / sizeof(uint8_t);
}

int CPacketDefine::GetSizeETX() const
{
    return sizeof(m_etx) / sizeof(uint8_t);
}

int CPacketDefine::GetSizeMainLength() const
{
    return sizeof(uint32_t);
}

int CPacketDefine::GetSizeBlockLength() const
{
    return sizeof(uint32_t);
}

uint8_t CPacketDefine::GetBlockDataDelimiter() const
{
    return ':';
}

uint8_t CPacketDefine::GetStartBlockDelimiter() const
{
    return NULL;
}

bool CPacketDefine::HasSTX(uint8_t* packet) const
{
    bool bRet = true;
    int  nLen = GetSizeSTX();

    for (int ii = 0; ii < nLen; ii++)
    {
        if (packet[ii] != GetSTX(ii))
        {
            bRet = false;
            break;
        }
    }

    return bRet;
}

bool CPacketDefine::HasETX(uint8_t* packet, int main_length) const
{
    bool bRet = true;
    int  nLen = GetSizeETX();
    uint8_t* pETX = &packet[GetPosETX(main_length)];

    for (int ii = 0; ii < nLen; ii++)
    {
        if (pETX[ii] != GetETX(ii))
        {
            bRet = false;
            break;
        }
    }

    return bRet;
}

uint32_t CPacketDefine::GetMainLength(uint8_t* packet) const
{
    uint32_t nLen = 0;
    memcpy(&nLen, &packet[GetPosMainLength()], GetSizeMainLength());
    return nLen;
}

std::string CPacketDefine::GetMainName(uint8_t* packet) const
{
    std::string name;
    int ii = GetPosMainName();
    do
    {
        name.push_back(packet[ii++]);
    } while (packet[ii] != GetStartBlockDelimiter());

    return name;
}

uint32_t CPacketDefine::GetBlockLength(uint8_t* block) const
{
    uint32_t nLen = 0;
    memcpy(&nLen, &block[0], GetSizeBlockLength());
    return nLen;
}

int CPacketDefine::GetBlock(uint8_t* block, BlockItem& item) const
{
    // Block 길이 추출
    uint32_t nLen = GetBlockLength(block);

    // Block 이름 추출   
    int ii = GetSizeBlockLength();
    do
    {
        item.name.push_back(block[ii++]);
    } while (block[ii] != GetBlockDataDelimiter());

    // Block Data 추출
    ii++; // BlockDataDelimiter 다음 위치로 이동
    item.data = &block[ii];
    item.length = nLen - (GetSizeBlockLength() + sizeof(uint8_t) + item.name.length());

    return 0;
}

//////////////////////////////////////////////////////////////////////////
CPacketWriter::CPacketWriter()
{

}

CPacketWriter::CPacketWriter(std::vector<uint8_t>* buffer)
    : m_buffer(buffer)
{

}

CPacketWriter::CPacketWriter(std::vector<uint8_t>* buffer, size_t size)
{
    m_buffer = buffer;
    if (m_buffer->size() < size)
        m_buffer->resize(size, 0);
}

CPacketWriter::~CPacketWriter()
{

}

void CPacketWriter::SetBuffer(std::vector<uint8_t>* buffer)
{
    m_buffer = buffer;
}

std::vector<uint8_t>* CPacketWriter::GetBuffer() const
{
    return m_buffer;
}

uint8_t* CPacketWriter::GetBufferPtr() const
{
    return m_buffer->data();
}

size_t CPacketWriter::GetBufferLength() const
{
    return m_buffer->size();
}

size_t CPacketWriter::GetPos() const
{
    return m_pos;
}

void CPacketWriter::Clear()
{
    m_pos = 0;
}

size_t CPacketWriter::GetBlockHeaderLength(const std::string& name)
{
    return (GetSizeBlockLength() + name.length() + sizeof(uint8_t));
}

int CPacketWriter::CheckBuffer(size_t len)
{
    if (nullptr == m_buffer)
        return 1;

    if (m_pos + len > m_buffer->size())   // buffer 의 크기가 부족하면 추가 할당
        m_buffer->resize(m_buffer->size() + len);

    return 0;
}

void CPacketWriter::WriteCurBuffer(void const* src, size_t len)
{
    memcpy(GetCurBufferPtr(), src, len);
    m_pos += len;
}

void CPacketWriter::WriteBuffer(int pos, void const* src, size_t len)
{
    memcpy(&(GetBufferPtr()[pos]), src, len);
}

uint8_t* CPacketWriter::GetCurBufferPtr() const
{
    return &(GetBufferPtr()[m_pos]);
}

void CPacketWriter::BufferPushBack(uint8_t data)
{
    WriteCurBuffer(&data, sizeof(data));
}

size_t CPacketWriter::WriteBlockName(const std::string& name)
{
    // Block Name 쓰기
    WriteCurBuffer(name.c_str(), name.length());

    // Block Name 과 Data 를 구분할 수 있는 구분자를 삽입
    BufferPushBack(GetBlockDataDelimiter());

    return m_pos;
}

std::string CPacketWriter::GetMainName() const
{
    if (nullptr == m_buffer)   return std::string();

    return CPacketDefine::GetMainName(m_buffer->data());
}

int CPacketWriter::Start(const std::string& name)
{
    if (nullptr == m_buffer)   return 0;

    if (m_buffer->empty() || m_buffer->size() < 1024)
        m_buffer->resize(1024, 0);    // Default 크기를 할당 함

    m_pos = 0;

    // STX 쓰기
    for (int ii = 0; ii < GetSizeSTX(); ii++)
        BufferPushBack(GetSTX(ii));

    // Main Length 공간 남겨두기
    m_pos += GetSizeMainLength();

    // Main Name 쓰기
    WriteCurBuffer(name.c_str(), name.length());

    BufferPushBack(GetStartBlockDelimiter());

    return m_pos;
}

int CPacketWriter::Write(const std::string& name, int32_t data)
{
    std::string temp = std::to_string(data);
    return Write(name, temp);
}

int CPacketWriter::Write(const std::string& name, uint32_t data)
{
    std::string temp = std::to_string(data);
    return Write(name, temp);
}

int CPacketWriter::Write(const std::string& name, int64_t data)
{
    std::string temp = std::to_string(data);
    return Write(name, temp);
}

int CPacketWriter::Write(const std::string& name, uint64_t data)
{
    std::string temp = std::to_string(data);
    return Write(name, temp);
}

int CPacketWriter::Write(const std::string& name, long data)
{
    std::string temp = std::to_string(data);
    return Write(name, temp);
}

int CPacketWriter::Write(const std::string& name, bool data)
{
    std::string temp = std::to_string(data);
    return Write(name, temp);
}

int CPacketWriter::Write(const std::string& name, float data)
{
    // 소수점 5자리 까지
    std::stringstream stream;
    stream << std::fixed << std::setprecision(5) << data;

    std::string temp = stream.str();
    return Write(name, temp);
}

int CPacketWriter::Write(const std::string& name, double data)
{
    // 소수점 5자리 까지
    std::stringstream stream;
    stream << std::fixed << std::setprecision(5) << data;

    std::string temp = stream.str();
    return Write(name, temp);
}

int CPacketWriter::Write(const std::string& name, uint8_t* data, int len)
{
    uint32_t block_len = GetBlockHeaderLength(name) + len;
    if (CheckBuffer(block_len))
        return 0;

    // Block Length 쓰기
    WriteCurBuffer(&block_len, sizeof(block_len));

    // Block Name 쓰기
    WriteBlockName(name);

    // Block Data 쓰기
    WriteCurBuffer(data, len);

    return m_pos;
}

int CPacketWriter::Write(const std::string& name, int len, std::function<void(void* data)> func)
{
    uint32_t block_len = GetBlockHeaderLength(name) + len;
    if (CheckBuffer(block_len))
        return 0;

    // Block Length 쓰기
    WriteCurBuffer(&block_len, sizeof(block_len));

    // Block Name 쓰기
    WriteBlockName(name);

    // Block Data 쓰기
    func(GetCurBufferPtr());
    m_pos += len;

    return m_pos;
}

int CPacketWriter::Write(const std::string& name, const std::string& data)
{
    uint32_t block_len = GetBlockHeaderLength(name) + data.length();
    if (CheckBuffer(block_len))
        return 0;

    // Block Length 쓰기
    WriteCurBuffer(&block_len, sizeof(block_len));

    // Block Name 쓰기
    WriteBlockName(name);

    // Block Data 쓰기
    WriteCurBuffer(data.c_str(), data.length());

    return m_pos;
}

int CPacketWriter::End()
{
    if (nullptr == m_buffer)
        return 0;

    for (int ii = 0; ii < GetSizeETX(); ii++)
        BufferPushBack(GetETX(ii));

    // Packet 의 전체 길이를 쓰기 한다.
    WriteBuffer(GetPosMainLength(), &m_pos, GetSizeMainLength());

    return m_pos;
}

//////////////////////////////////////////////////////////////////////////

CPacketReader::CPacketReader()
{

}

CPacketReader::~CPacketReader()
{
    Clear();
}

void CPacketReader::Clear()
{
    m_main_name.clear();
    m_block_items.clear();
}

int CPacketReader::DoAnalyze(uint8_t* packet, int len)
{
    Clear();

    if (!HasSTX(packet))         return 1;
    if (!HasETX(packet, len))    return 2;

    m_main_name = GetMainName(packet);
    if (m_main_name.empty())     return 3;

    int nEndPos = GetPosSTX() + GetMainLength(packet) - GetSizeETX();
    int nPos = GetPosMainName() + m_main_name.length() + sizeof(uint8_t); // 첫 시작 Block 위치

    while (nPos < nEndPos)
    {
        uint8_t* block = &packet[nPos];
        uint32_t nLen = GetBlockLength(block);

        BlockItem item;
        GetBlock(block, item);
        m_block_items.push_back(std::move(item));

        nPos += nLen;
    }

    return 0;
}

std::string CPacketReader::GetReadMainName() const
{
    return m_main_name;
}

bool CPacketReader::HasReadMainName() const
{
    if (m_main_name.empty())
        return false;
    return true;
}

int CPacketReader::ReadLength(const std::string& name) const
{
    BlockItem data;

    if (Read(name, data))
        return 0;

    return data.length;
}

int CPacketReader::Read(const std::string& name, BlockItem& data) const
{
    int nRet = 1;

    auto iter = m_block_items.cbegin();
    while (iter != m_block_items.cend())
    {
        const BlockItem& item = *iter;
        if (name == item.name)
        {
            data = item;
            nRet = 0;
            break;
        }
        iter++;
    }

    return nRet;
}

int CPacketReader::Read(const std::string& name, int32_t& data) const
{
    std::string temp;
    if (Read(name, temp))   return 1;
    data = std::stoi(temp);

    return 0;
}

int CPacketReader::Read(const std::string& name, uint32_t& data) const
{
    std::string temp;
    if (Read(name, temp))   return 1;
    data = std::stoi(temp);

    return 0;
}

int CPacketReader::Read(const std::string& name, int64_t& data) const
{
    std::string temp;
    if (Read(name, temp))   return 1;
    data = std::stoi(temp);

    return 0;
}

int CPacketReader::Read(const std::string& name, uint64_t& data) const
{
    std::string temp;
    if (Read(name, temp))   return 1;
    data = std::stoi(temp);

    return 0;
}

int CPacketReader::Read(const std::string& name, long& data) const
{
    std::string temp;
    if (Read(name, temp))   return 1;
    data = std::stol(temp);

    return 0;
}

int CPacketReader::Read(const std::string& name, bool& data) const
{
    std::string temp;
    if (Read(name, temp))   return 1;
    data = std::stoi(temp);

    return 0;
}

int CPacketReader::Read(const std::string& name, float& data) const
{
    std::string temp;
    if (Read(name, temp))   return 1;
    data = std::stof(temp);

    return 0;
}

int CPacketReader::Read(const std::string& name, double& data) const
{
    std::string temp;
    if (Read(name, temp))   return 1;
    data = std::stod(temp);

    return 0;
}


int CPacketReader::Read(const std::string& name, std::string& data) const
{
    BlockItem item;
    if (Read(name, item))   return 1;

    data.insert(0, reinterpret_cast<const char *>(item.data), item.length);

    return 0;
}

int CPacketReader::Read(const std::string& name, uint8_t*& data) const
{
    BlockItem item;
    if (Read(name, item))   return 1;

    data = item.data;

    return 0;
}

int CPacketReader::ReadMemCopy(const std::string& name, uint8_t* data) const
{
    BlockItem item;
    if (Read(name, item))   return 1;

    memcpy(data, item.data, item.length);

    return 0;
}

int CPacketReader::ReadMemCopy(const std::string& name, std::vector<uint8_t>& data) const
{
    BlockItem item;
    if (Read(name, item))   return 1;

    int data_len = ReadLength(name);
    if (data.size() < data_len)
        data.resize(data_len);

    memcpy(data.data(), item.data, data_len);

    return 0;
}

// Test Code
int Test_Packet()
{
    int ret = 0;

    std::vector<uint8_t> buffer;
    std::string main_name = "LPREngineInfo";

    CPacketWriter writer(&buffer);
    writer.Start(main_name);
    writer.Write("EngineID", 1);
    writer.Write("Width", 1920);
    writer.Write("Height", 1080);
    int packet_len = writer.End();

    //////////////////////////////////////////////////////////////////////////
    CPacketReader reader;
    reader.DoAnalyze(buffer.data(), packet_len);

    try
    {
        if (main_name != reader.GetReadMainName())
            throw 1;

        int nEngineID = 0, nWidth = 0, nHeight = 0;
        reader.Read("EngineID", nEngineID);
        reader.Read("Width", nWidth);
        reader.Read("Height", nHeight);

        if (1 != nEngineID)     throw 2;
        if (1920 != nWidth)     throw 3;
        if (1080 != nHeight)    throw 4;
    }
    catch (int code)
    {
        ret = code;
    }

    return ret;
}

#pragma warning (pop)