#pragma once

///  @author  Lee Jong Oh
///  @brief   유틸리티 용으로 사용되는 api 모듬

#include <string>
#include <thread>


int DiffTime(const timeval& start, const timeval& end);

///////////////////////////////////////////////////////////////////////////////
// Process
///////////////////////////////////////////////////////////////////////////////

// 현재 실행중인 프로세스 절대 경로를 반환 한다.
int  GetProcessPath(std::string& dest);
int  GetProcessPath(std::wstring& dest);

// 파일 이름이 포함된 절대 경로 path 전달하면 파일 이름만 return 한다.
std::string  GetExtractFileName(const std::string& path);
std::wstring GetExtractFileName(const std::wstring& path);

// 파일 이름이 포함된 절대 경로 path 전달하면 파일 이름을 제외한 경로를 return 한다.
std::string  GetExtractPath(const std::string& path);
std::wstring GetExtractPath(const std::wstring& path);

// thread 의 이름을 설정 한다 (Debug 용도)
void SetThreadName(const std::string& name);
void SetThreadName(std::thread& thread, const std::string& name);

///////////////////////////////////////////////////////////////////////////////
// String & Path
///////////////////////////////////////////////////////////////////////////////
void CreateDirectoryD(const std::string& path);
void CreateDirectoryD(const std::wstring& path, const std::wstring& dir_name = L"");
bool IsExistFile(const std::string& fila_name);       // 파일이 존재하는지 확인한다.
bool IsExistFile(const std::wstring& fila_name);
bool IsExistDirectory(const std::string& path);       // 디렉토리가 존재하는지 확인 한다.
bool IsExistDirectory(const std::wstring& path);

// Unicode -> std::vector<uint8_t> 변환
std::vector<uint8_t> convert_vector(const std::wstring& src);
// Multibyte -> std::vector<uint8_t> 변환
std::vector<uint8_t> convert_vector(const std::string& src);

// Multibyte ,Unicode, UTF8 문자열 변환 함수
std::string  convert_unicode_to_utf8  (const std::wstring& src_unicode);
std::wstring convert_utf8_to_unicode  (const std::string&  src_utf8);
std::wstring convert_muti_to_unicode  (const std::string&  src_multi);
std::string  convert_unicode_to_multi (const std::wstring& src_unicode);

// Multibyte 변환 함수
std::string  convert_multi   (const std::string&  src);
std::string  convert_multi   (const std::wstring& src);
// Unicode 변환 함수
std::wstring convert_unicode (const std::string&  src);
std::wstring convert_unicode (const std::wstring& src);

// _UNICODE 정의에 구애 받지 않고 공통된 인터페이스 제공을 위해 함수 오버로딩 기법을 사용해서 제공해주는 함수
// Multibyte <-> Unicode 문자열 변환 함수
void convert_string(const std::wstring& src, std::wstring& dst);
void convert_string(const std::wstring& src, std::string&  dst);
void convert_string(const std::string&  src, std::string&  dst);
void convert_string(const std::string&  src, std::wstring& dst);

// message 문자열에서 pattern 이 되는 문자열을 replace 로 변환
std::string  replace_all(const std::string &message, const std::string &pattern, const std::string &replace);
std::wstring replace_all(const std::wstring &message, const std::wstring &pattern, const std::wstring &replace);

// 문자열을 구분자를 기준으로 분리 한다.
// param str : 원본 문자열
// param delimiter : 구분자
// param out_split : 구분자를 기준으로 분리된 문자열을 저장하여 반환
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

