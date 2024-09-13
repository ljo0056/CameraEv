#include "stdafx.h"
#include "Util.h"
#include <processthreadsapi.h>

int DiffTime(const timeval& start, const timeval& end)
{
    return (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000);
}

int GetProcessPath(std::string& dest)
{
    std::string temp;
    char path[256] = { 0, };
    GetModuleFileNameA(NULL, path, 256);

    temp = path;
    std::string::size_type pos = temp.rfind('\\');
    if (std::string::npos == pos)
        return 1;

    dest = temp.substr(0, pos);

    return 0;
}

int GetProcessPath(std::wstring& dest)
{
    std::wstring temp;
    wchar_t path[256] = { 0, };
    GetModuleFileNameW(NULL, path, 256);

    temp = path;
    std::wstring::size_type pos = temp.rfind('\\');
    if (std::wstring::npos == pos)
        return 1;

    dest = temp.substr(0, pos);

    return 0;
}

std::string GetExtractFileName(const std::string& path)
{
    std::string::size_type pos = path.rfind('\\');
    if (std::string::npos == pos)
        return std::string();

    return path.substr(pos + 1, path.length() - (pos + 1));
}

std::wstring GetExtractFileName(const std::wstring& path)
{
    std::wstring::size_type pos = path.rfind('\\');
    if (std::wstring::npos == pos)
        return std::wstring();

    return path.substr(pos + 1, path.length() - (pos + 1));
}

std::string GetExtractPath(const std::string& path)
{
    std::string::size_type pos = path.rfind('\\');
    if (std::string::npos == pos)
        return std::string();

    return path.substr(0, pos);
}

std::wstring GetExtractPath(const std::wstring& path)
{
    std::wstring::size_type pos = path.rfind('\\');
    if (std::wstring::npos == pos)
        return std::wstring();

    return path.substr(0, pos);
}

//////////////////////////////////////////////////////////////////////////
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD    dwType;     // Must be 0x1000.
    LPCSTR   szName;     // Pointer to name (in user addr space).
    DWORD    dwThreadID; // Thread ID (-1=caller thread).
    DWORD    dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(const std::string& name)
{
#ifdef _DEBUG
    HRESULT r = SetThreadDescription(GetCurrentThread(), convert_muti_to_unicode(name).c_str());
    if (FAILED(r)) {
        // Failed...
    }
#endif
}

void SetThreadName(std::thread& thread, const std::string& name)
{
#ifdef _DEBUG
    HANDLE thread_h = static_cast<HANDLE>(thread.native_handle());
    HRESULT r = SetThreadDescription(thread_h, convert_muti_to_unicode(name).c_str());
    if (FAILED(r)) {
        // Failed...
    }
#endif
}
//////////////////////////////////////////////////////////////////////////

bool IsExistFile(const std::string& fila_name)
{
    if (INVALID_FILE_ATTRIBUTES == GetFileAttributesA(fila_name.c_str()))
        return false;

    return true;
}

bool IsExistFile(const std::wstring& fila_name)
{
    if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(fila_name.c_str()))
        return false;

    return true;
}

bool IsExistDirectory(const std::string& path)
{
    _finddata_t c_file;
    intptr_t hFile;
    bool result = false;

    hFile = _findfirst(path.c_str(), &c_file);
    if (c_file.attrib & _A_SUBDIR) 
        result = true;
    _findclose(hFile);

    return result;
}

bool IsExistDirectory(const std::wstring& path)
{
    _wfinddata_t c_file;
    intptr_t hFile;
    bool result = false;

    hFile = _wfindfirst(path.c_str(), &c_file);
    if (c_file.attrib & _A_SUBDIR)
        result = true;
    _findclose(hFile);

    return result;
}

void CreateDirectoryD(const std::string& path)
{
    std::vector<std::string> vec_split;
    split_string(path, '\\', vec_split);

    if (vec_split.size() < 2)
        return;

    std::string add_path = vec_split[0];
    add_path.push_back('\\');

    vec_split.erase(vec_split.begin());

    for (const std::string& t : vec_split)
    {
        add_path += t;
        add_path.push_back('\\');

        CreateDirectoryA(add_path.c_str(), NULL);
    }
}

void CreateDirectoryD(const std::wstring& path, const std::wstring& dir_name)
{
    std::vector<std::wstring> vec_split;
    split_string(path, '\\', vec_split);

    if (vec_split.size() < 2)
        return;

    std::wstring add_path = vec_split[0];
    add_path.push_back('\\');

    vec_split.erase(vec_split.begin());

    for (const std::wstring& t : vec_split)
    {
        add_path += t;
        add_path.push_back('\\');

        CreateDirectoryW(add_path.c_str(), NULL);
    }
}

std::vector<uint8_t> convert_vector(const std::wstring& src)
{
    std::vector<uint8_t> vec(src.length() * sizeof(wchar_t));
    memcpy(vec.data(), src.data(), vec.size());

    return vec;
}

std::vector<uint8_t> convert_vector(const std::string& src)
{
    std::vector<uint8_t> vec(src.length() * sizeof(char));
    memcpy(vec.data(), src.data(), vec.size());

    return vec;
}

std::string convert_unicode_to_utf8(const std::wstring& src_unicode)
{
    int n = WideCharToMultiByte(CP_UTF8, 0, src_unicode.c_str(), (int)src_unicode.length(), NULL, 0, NULL, NULL);

    std::string dst_utf8(n, 0);
    WideCharToMultiByte(CP_UTF8, 0, src_unicode.c_str(), (int)src_unicode.length(), (LPSTR)dst_utf8.data(), n, NULL, NULL);

    return dst_utf8;
}

std::wstring convert_utf8_to_unicode(const std::string& src_utf8)
{
    int n = MultiByteToWideChar(CP_UTF8, 0, src_utf8.c_str(), (int)src_utf8.length(), NULL, 0);

    std::wstring dst_unicode(n, 0);
    MultiByteToWideChar(CP_UTF8, 0, src_utf8.c_str(), (int)src_utf8.length(), (LPWSTR)dst_unicode.data(), n);

    return std::wstring(dst_unicode);
}

std::wstring convert_muti_to_unicode(const std::string& src_multi)
{
    int n = MultiByteToWideChar(CP_ACP, 0, src_multi.c_str(), (int)src_multi.length(), NULL, 0);

    std::wstring dst_unicode(n, 0);
    MultiByteToWideChar(CP_ACP, 0, src_multi.c_str(), (int)src_multi.length(), (LPWSTR)dst_unicode.data(), n);

    return dst_unicode;
}

std::string convert_unicode_to_multi(const std::wstring& src_unicode)
{
    int n = WideCharToMultiByte(CP_ACP, 0, src_unicode.c_str(), (int)src_unicode.length(), NULL, 0, NULL, NULL);

    std::string dst_unicode(n, 0);
    WideCharToMultiByte(CP_ACP, 0, src_unicode.c_str(), (int)src_unicode.length(), (LPSTR)dst_unicode.data(), n, NULL, NULL);

    return std::string(dst_unicode);
}

std::string convert_multi(const std::string& src)
{
    if (src.empty())
        return std::string("");

    std::string dst;
    dst.assign(src.begin(), src.end());

    return dst;
}

std::string convert_multi(const std::wstring& src)
{
    if (src.empty())
        return std::string("");

    return convert_unicode_to_multi(src);
}

std::wstring convert_unicode(const std::string& src)
{
    if (src.empty())
        return std::wstring(L"");

    return convert_muti_to_unicode(src);
}

std::wstring convert_unicode(const std::wstring& src)
{
    if (src.empty())
        return std::wstring(L"");

    std::wstring dst;
    dst.assign(src.begin(), src.end());

    return dst;
}

void convert_string(const std::wstring& src, std::wstring& dst)
{
    if (src.empty())
        return;

    dst = convert_unicode(src);
}

void convert_string(const std::wstring& src, std::string&  dst)
{
    if (src.empty())
        return;

    dst = convert_multi(src);
}

void convert_string(const std::string&  src, std::string&  dst)
{
    if (src.empty())
        return;

    dst = convert_multi(src);
}

void convert_string(const std::string&  src, std::wstring& dst)
{
    if (src.empty())
        return;

    dst = convert_unicode(src);
}

std::string replace_all(const std::string &message, const std::string &pattern, const std::string &replace)
{
    std::string result = message;
    std::string::size_type pos = 0;
    std::string::size_type offset = 0;

    while ((pos = result.find(pattern, offset)) != std::string::npos)
    {
        result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
        offset = pos + replace.size();
    }

    return result;
}

std::wstring replace_all(const std::wstring &message, const std::wstring &pattern, const std::wstring &replace)
{
    std::wstring result = message;
    std::wstring::size_type pos = 0;
    std::wstring::size_type offset = 0;

    while ((pos = result.find(pattern, offset)) != std::string::npos)
    {
        result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
        offset = pos + replace.size();
    }

    return result;
}

int split_string(const std::string& str, const std::string& seperator, std::vector<std::string>& out_split)
{
    std::string::size_type prev_pos = 0, pos = 0;

    while ((pos = str.find(seperator, pos)) != std::string::npos)
    {
        std::string substring(str.substr(prev_pos, pos - prev_pos));
        out_split.push_back(substring);
        pos += seperator.length();
        prev_pos = pos;
    }

    out_split.push_back(str.substr(prev_pos, pos - prev_pos)); // Last word

    return 0;
}

int split_string(const std::wstring& str, const std::wstring& seperator, std::vector<std::wstring>& out_split)
{
    std::wstring::size_type prev_pos = 0, pos = 0;

    while ((pos = str.find(seperator, pos)) != std::wstring::npos)
    {
        std::wstring substring(str.substr(prev_pos, pos - prev_pos));
        out_split.push_back(substring);
        pos += seperator.length();
        prev_pos = pos;
    }

    out_split.push_back(str.substr(prev_pos, pos - prev_pos)); // Last word

    return 0;
}

int split_string(const std::string& str, char delimiter, std::vector<std::string>& out_split)
{
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter))
        out_split.push_back(token);

    return 0;
}

int split_string(const std::wstring& str, wchar_t delimiter, std::vector<std::wstring>& out_split)
{
    std::wistringstream iss(str);
    std::wstring token;
    while (std::getline(iss, token, delimiter))
        out_split.push_back(token);

    return 0;
}

BOOL IsPhysMemAvail(DWORD nRequiredFreePhysMem)
{
    if (GetAvailPhysMemSize() < nRequiredFreePhysMem)
        return FALSE;
    return TRUE;
}

DWORD GetAvailPhysMemSize()
{
    MEMORYSTATUS statex;
    GlobalMemoryStatus(&statex);
    return (DWORD)statex.dwAvailPhys;
}

DWORD GetTotalPhysMemSize()
{
    MEMORYSTATUS statex;
    GlobalMemoryStatus(&statex);
    return (DWORD)statex.dwTotalPhys;
}

DWORD GetAvailVirtMemSize()
{
    MEMORYSTATUS statex;
    GlobalMemoryStatus(&statex);
    return (DWORD)statex.dwAvailVirtual;
}

DWORD GetTotalVirtMemSize()
{
    MEMORYSTATUS statex;
    GlobalMemoryStatus(&statex);
    return (DWORD)statex.dwTotalVirtual;
}

float GetAvailPhysMemSizeInMB()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return float(statex.ullAvailPhys / (1024 * 1024));
}

float GetTotalPhysMemSizeInMB()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return float(statex.ullTotalPhys / (1024 * 1024));
}

float GetAvailVirtMemSizeInMB()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return float(statex.ullAvailVirtual / (1024 * 1024));
}


float GetTotalVirtMemSizeInMB()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return float(statex.ullTotalVirtual / (1024 * 1024));
}

float GetAvailPhysMemSizeInGB()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return statex.ullAvailPhys / (1024 * 1024 * 1024.0f);
}

float GetTotalPhysMemSizeInGB()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return statex.ullTotalPhys / (1024 * 1024 * 1024.0f);
}

float GetAvailVirtMemSizeInGB()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return statex.ullAvailVirtual / (1024 * 1024 * 1024.0f);
}


float GetTotalVirtMemSizeInGB()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return statex.ullTotalVirtual / (1024 * 1024 * 1024.0f);
}
