#pragma once

///  @class   WGDIPlus
///  @brief   GDI++ 의 초기화 및 유틸리티 class 모듈
///  @author  Lee Jong Oh

#include <gdiplus.h>

//////////////////////////////////////////////////////////////////////////

class WGDIPlus
{
private:
    ULONG_PTR   m_token;
    bool        m_vaild;

public:
    WGDIPlus();
    ~WGDIPlus();

    // sample code...
    // Gdiplus::Image* m_pImgNomal = nullptr;
    // BGDIPlus::LoadFromResource(m_pImgLogo, (LPCTSTR)MAKEINTRESOURCE(IDB_PNG_LOGO), _T("PNG"), AfxGetInstanceHandle());
    // delete m_pImgNomal;
    static BOOL LoadFromResource(Gdiplus::Image*& img, LPCTSTR pName, LPCTSTR pType, HMODULE hInst);
};

