#pragma once

class MiniDump
{
public:
    static CString  m_strPath;
    static int      m_nDumpLevel;
    static BOOL     m_bSaveDump;

public:
    MiniDump();

    static void EnableHotFix();

    static BOOL Begin(const CString& strPath);
    static BOOL End();
    //static void DeleteDumpForDay(int nDeleteDay);
};
