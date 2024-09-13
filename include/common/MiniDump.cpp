#include "stdafx.h"
#include "MiniDump.h"
#include "Util.h"
#pragma warning (disable:4091)
#include <dbghelp.h>


typedef BOOL(WINAPI *MINIDUMPWRITEDUMP)( // Callback 함수의 원형
    HANDLE hProcess,
    DWORD  dwPid,
    HANDLE hFile,
    MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION     ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION   UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION      CallbackParam);

LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter = NULL;

#define PROCESS_CALLBACK_FILTER_ENABLED     0x1
typedef BOOL(WINAPI* PGPUMEP)(LPDWORD lpFlags);
typedef BOOL(WINAPI* PSPUMEP)(DWORD dwFlags);

MiniDump::MiniDump()
{

}

void MiniDump::EnableHotFix()
{
    HMODULE hDLL = LoadLibrary(_T("Kernel32.dll"));

    if (hDLL)
    {
        PGPUMEP pGetProcessUserModeExceptionPolicy =
            (PGPUMEP)GetProcAddress(hDLL, ("GetProcessUserModeExceptionPolicy"));

        PSPUMEP pSetProcessUserModeExceptionPolicy =
            (PSPUMEP)GetProcAddress(hDLL, ("SetProcessUserModeExceptionPolicy"));

        if (pGetProcessUserModeExceptionPolicy && pSetProcessUserModeExceptionPolicy)
        {
            DWORD dwFlags;
            if (pGetProcessUserModeExceptionPolicy(&dwFlags))
            {
                pSetProcessUserModeExceptionPolicy(
                    dwFlags & ~PROCESS_CALLBACK_FILTER_ENABLED);
            }
        }

        FreeLibrary(hDLL);
    }
}

CString GetExceptionCodeString(int nExceptionCode)
{
    CString strECode;
    if (nExceptionCode == EXCEPTION_ACCESS_VIOLATION)               strECode = "Access Violation";
    else if (nExceptionCode == EXCEPTION_DATATYPE_MISALIGNMENT)     strECode = "Data Type Misalignment";
    else if (nExceptionCode == EXCEPTION_BREAKPOINT)                strECode = "Break Point";
    else if (nExceptionCode == EXCEPTION_SINGLE_STEP)               strECode = "Single Step";
    else if (nExceptionCode == EXCEPTION_ARRAY_BOUNDS_EXCEEDED)     strECode = "Array Bounds Exceeded";
    else if (nExceptionCode == EXCEPTION_FLT_DENORMAL_OPERAND)      strECode = "Float Denomal Operand";
    else if (nExceptionCode == EXCEPTION_FLT_DIVIDE_BY_ZERO)        strECode = "Float DIvide By Zero";
    else if (nExceptionCode == EXCEPTION_FLT_INEXACT_RESULT)        strECode = "Float Inexact Result";
    else if (nExceptionCode == EXCEPTION_FLT_INVALID_OPERATION)     strECode = "Float Invalid Operation";
    else if (nExceptionCode == EXCEPTION_FLT_OVERFLOW)              strECode = "Float Overflow";
    else if (nExceptionCode == EXCEPTION_FLT_STACK_CHECK)           strECode = "Float Stack Check";
    else if (nExceptionCode == EXCEPTION_FLT_UNDERFLOW)             strECode = "Float Underflow";
    else if (nExceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO)        strECode = "Int Divide By Zero";
    else if (nExceptionCode == EXCEPTION_INT_OVERFLOW)              strECode = "Int Overflow";
    else if (nExceptionCode == EXCEPTION_PRIV_INSTRUCTION)          strECode = "Privileged Instruction";
    else if (nExceptionCode == EXCEPTION_IN_PAGE_ERROR)             strECode = "In Page Error";
    else if (nExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION)       strECode = "Illegal Instruction";
    else if (nExceptionCode == EXCEPTION_NONCONTINUABLE_EXCEPTION)  strECode = "Noncontiuable Exception";
    else if (nExceptionCode == EXCEPTION_STACK_OVERFLOW)            strECode = "Stack Overflow";
    else if (nExceptionCode == EXCEPTION_INVALID_DISPOSITION)       strECode = "Invalid Disposition";
    else if (nExceptionCode == EXCEPTION_GUARD_PAGE)                strECode = "Guard Page";
    else if (nExceptionCode == EXCEPTION_INVALID_HANDLE)            strECode = "Invalid Handle";
    else                                                            strECode = "Unknown";
    return strECode;
}

CString GetExeName()
{
    TCHAR szModuleName[MAX_PATH] = { 0, };
    GetModuleFileName(NULL, szModuleName, _countof(szModuleName));
    CString strModuleName = szModuleName;

    int nPos = strModuleName.ReverseFind('\\');
    int nLen = strModuleName.GetLength();
    strModuleName = strModuleName.Mid(nPos + 1, nLen - (nPos - 1));

    nPos = strModuleName.ReverseFind('.');
    strModuleName = strModuleName.Left(nPos);

    return strModuleName;
}

LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo)
{
    HMODULE	DllHandle = NULL;

    // Windows 2000 이전에는 따로 DBGHELP를 배포해서 설정해 주어야 한다.
    DllHandle = LoadLibrary(_T("DBGHELP.DLL"));

    if (DllHandle)
    {
        MINIDUMPWRITEDUMP Dump = (MINIDUMPWRITEDUMP)GetProcAddress(DllHandle, "MiniDumpWriteDump");

        if (Dump)
        {
            CString strModuleName = GetExeName();

            TCHAR      DumpPath[MAX_PATH] = { 0, };
            SYSTEMTIME SystemTime;

            GetLocalTime(&SystemTime);

            _sntprintf(DumpPath, MAX_PATH, _T("%s\\%s_%04d-%02d-%02d_%02d-%02d-%02d.dmp"),
                MiniDump::m_strPath.GetBuffer(0),
                strModuleName.GetBuffer(0),
                SystemTime.wYear,
                SystemTime.wMonth,
                SystemTime.wDay,
                SystemTime.wHour,
                SystemTime.wMinute,
                SystemTime.wSecond);

            int nECode = exceptionInfo->ExceptionRecord->ExceptionCode;
            CString strExceptionCode = GetExceptionCodeString(nECode);

            logi(_T("--------------------------------------------------------------------------\n"));
            logi(_T(" Dump Path           : %s"), DumpPath);
            logi(_T(" Exception Address   : %08x"), exceptionInfo->ExceptionRecord->ExceptionAddress);
            logi(_T(" Exception Code      : %08x (%s)"), nECode, strExceptionCode);
            logi(_T("--------------------------------------------------------------------------\n"));

            HANDLE FileHandle = INVALID_HANDLE_VALUE;

            if (MiniDump::m_bSaveDump)
            {
                FileHandle = CreateFile(
                    DumpPath,
                    GENERIC_WRITE,
                    FILE_SHARE_WRITE,
                    NULL, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
            }

            if (FileHandle != INVALID_HANDLE_VALUE)
            {
                _MINIDUMP_EXCEPTION_INFORMATION MiniDumpExceptionInfo;

                MiniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
                MiniDumpExceptionInfo.ExceptionPointers = exceptionInfo;
                MiniDumpExceptionInfo.ClientPointers = NULL;

                INT nDumpLevel = MiniDump::m_nDumpLevel;

                BOOL Success = Dump(
                    GetCurrentProcess(),
                    GetCurrentProcessId(),
                    FileHandle,
                    (MINIDUMP_TYPE)nDumpLevel,
                    &MiniDumpExceptionInfo,
                    NULL,
                    NULL);

                CloseHandle(FileHandle);

                if (Success)
                {
                    TerminateProcess(GetCurrentProcess(), -1);
                    return EXCEPTION_EXECUTE_HANDLER;
                }
            }

            CloseHandle(FileHandle);
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

//////////////////////////////////////////////////////////////////////////
//
// class MiniDump
//
//////////////////////////////////////////////////////////////////////////

CString MiniDump::m_strPath = _T("");
int     MiniDump::m_nDumpLevel = MiniDumpNormal;
BOOL    MiniDump::m_bSaveDump = TRUE;

BOOL MiniDump::Begin(const CString& strPath)
{
    m_strPath = strPath;
    CreateDirectory(strPath, NULL);

    EnableHotFix();

    SetErrorMode(SEM_NOGPFAULTERRORBOX);

    PreviousExceptionFilter = SetUnhandledExceptionFilter(UnHandledExceptionFilter);

    return true;
}

BOOL MiniDump::End()
{
    SetUnhandledExceptionFilter(PreviousExceptionFilter);

    return true;
}

//void MiniDump::DeleteDumpForDay(int nDeleteDay)
//{
//    WIN32_FIND_DATA FindData;
//
//    CString strModuleName = GetExeName();
//    CString strPath = m_strPath;
//    strPath += _T("\\*.dmp");
//
//    HANDLE hFind = FindFirstFile(strPath, &FindData);
//
//    SYSTEMTIME	SystemTime;
//    GetLocalTime(&SystemTime);
//    CTime tmThreshold(SystemTime);
//    tmThreshold -= CTimeSpan(nDeleteDay, 0, 0, 0);
//
//    if (INVALID_HANDLE_VALUE == hFind)	return;
//
//    CString strFormat;
//    strFormat.Format(_T("%s_%%04d-%%02d-%%02d_%%02d-%%02d-%%02d.dmp"), strModuleName);
//    int nYear = 0, nMonth = 0, nDay = 0;
//    int nHour = 0, nMinute = 0, nSecond = 0;
//    do
//    {
//        if (INVALID_HANDLE_VALUE == hFind)	return;
//
//        if (!_tcscmp(FindData.cFileName, _T(".")))   continue;
//        if (!_tcscmp(FindData.cFileName, _T("..")))  continue;
//
//        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//            continue;
//        int nRet = _stscanf(FindData.cFileName, strFormat, &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);
//        if (6 == nRet)
//        {
//            CTime tmLog(nYear, nMonth, nDay, 0, 0, 0);
//            if (tmThreshold > tmLog)
//            {
//                CString strFilePath;
//                strFilePath.Format(_T("%s\\%s"), m_strPath, FindData.cFileName);
//                DeleteFile(strFilePath);
//            }
//        }
//
//    } while (FindNextFile(hFind, &FindData));
//
//    FindClose(hFind);
//}
