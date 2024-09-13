#pragma once

///  @author  Lee Jong Oh
///  @brief   ��ƿ��Ƽ ������ ���Ǵ� api ���

#include <string>
#include <thread>


int DiffTime(const timeval& start, const timeval& end);

///////////////////////////////////////////////////////////////////////////////
// Process
///////////////////////////////////////////////////////////////////////////////

// ���� �������� ���μ��� ���� ��θ� ��ȯ �Ѵ�.
int  GetProcessPath(std::string& dest);
int  GetProcessPath(std::wstring& dest);

// ���� �̸��� ���Ե� ���� ��� path �����ϸ� ���� �̸��� return �Ѵ�.
std::string  GetExtractFileName(const std::string& path);
std::wstring GetExtractFileName(const std::wstring& path);

// ���� �̸��� ���Ե� ���� ��� path �����ϸ� ���� �̸��� ������ ��θ� return �Ѵ�.
std::string  GetExtractPath(const std::string& path);
std::wstring GetExtractPath(const std::wstring& path);

// thread �� �̸��� ���� �Ѵ� (Debug �뵵)
void SetThreadName(const std::string& name);
void SetThreadName(std::thread& thread, const std::string& name);

///////////////////////////////////////////////////////////////////////////////
// String & Path
///////////////////////////////////////////////////////////////////////////////
void CreateDirectoryD(const std::string& path);
void CreateDirectoryD(const std::wstring& path, const std::wstring& dir_name = L"");
bool IsExistFile(const std::string& fila_name);       // ������ �����ϴ��� Ȯ���Ѵ�.
bool IsExistFile(const std::wstring& fila_name);
bool IsExistDirectory(const std::string& path);       // ���丮�� �����ϴ��� Ȯ�� �Ѵ�.
bool IsExistDirectory(const std::wstring& path);

// Unicode -> std::vector<uint8_t> ��ȯ
std::vector<uint8_t> convert_vector(const std::wstring& src);
// Multibyte -> std::vector<uint8_t> ��ȯ
std::vector<uint8_t> convert_vector(const std::string& src);

// Multibyte ,Unicode, UTF8 ���ڿ� ��ȯ �Լ�
std::string  convert_unicode_to_utf8  (const std::wstring& src_unicode);
std::wstring convert_utf8_to_unicode  (const std::string&  src_utf8);
std::wstring convert_muti_to_unicode  (const std::string&  src_multi);
std::string  convert_unicode_to_multi (const std::wstring& src_unicode);

// Multibyte ��ȯ �Լ�
std::string  convert_multi   (const std::string&  src);
std::string  convert_multi   (const std::wstring& src);
// Unicode ��ȯ �Լ�
std::wstring convert_unicode (const std::string&  src);
std::wstring convert_unicode (const std::wstring& src);

// _UNICODE ���ǿ� ���� ���� �ʰ� ����� �������̽� ������ ���� �Լ� �����ε� ����� ����ؼ� �������ִ� �Լ�
// Multibyte <-> Unicode ���ڿ� ��ȯ �Լ�
void convert_string(const std::wstring& src, std::wstring& dst);
void convert_string(const std::wstring& src, std::string&  dst);
void convert_string(const std::string&  src, std::string&  dst);
void convert_string(const std::string&  src, std::wstring& dst);

// message ���ڿ����� pattern �� �Ǵ� ���ڿ��� replace �� ��ȯ
std::string  replace_all(const std::string &message, const std::string &pattern, const std::string &replace);
std::wstring replace_all(const std::wstring &message, const std::wstring &pattern, const std::wstring &replace);

// ���ڿ��� �����ڸ� �������� �и� �Ѵ�.
// param str : ���� ���ڿ�
// param delimiter : ������
// param out_split : �����ڸ� �������� �и��� ���ڿ��� �����Ͽ� ��ȯ
int split_string(const std::string& str, const std::string& seperator, std::vector<std::string>& out_split);
int split_string(const std::wstring& str, const std::wstring& seperator, std::vector<std::wstring>& out_split);
int split_string(const std::string& str, char delimiter, std::vector<std::string>& out_split);
int split_string(const std::wstring& str, wchar_t delimiter, std::vector<std::wstring>& out_split);

///////////////////////////////////////////////////////////////////////////////
// Memory Checking
///////////////////////////////////////////////////////////////////////////////

BOOL  IsPhysMemAvail(DWORD nRequiredFreePhysMem);
DWORD GetAvailPhysMemSize();
DWORD GetTotalPhysMemSize();
DWORD GetAvailVirtMemSize();
DWORD GetTotalVirtMemSize();
float GetAvailPhysMemSizeInMB();
float GetTotalPhysMemSizeInMB();
float GetAvailVirtMemSizeInMB();
float GetTotalVirtMemSizeInMB();
float GetAvailPhysMemSizeInGB();
float GetTotalPhysMemSizeInGB();
float GetAvailVirtMemSizeInGB();
float GetTotalVirtMemSizeInGB();

