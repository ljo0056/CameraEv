#include "stdafx.h"
#include "WGDIPlus.h"

#pragma comment(lib, "gdiplus.lib")

//////////////////////////////////////////////////////////////////////////

WGDIPlus::WGDIPlus(void)
    : m_token(0)
    , m_vaild(false)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    m_vaild = (Gdiplus::GdiplusStartup(&m_token, &gdiplusStartupInput, NULL) == Gdiplus::Ok);
}

WGDIPlus::~WGDIPlus(void)
{
    Gdiplus::GdiplusShutdown(m_token);
}

BOOL WGDIPlus::LoadFromResource(Gdiplus::Image*& img, LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
{
    IStream* pStream = NULL;

    HRSRC hResource = ::FindResource(hInst, pName, pType);
    if (!hResource)
        return FALSE;

    DWORD nImageSize = ::SizeofResource(hInst, hResource);
    if (!nImageSize)
        return FALSE;

    HGLOBAL m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, nImageSize);
    BYTE* pBytes = (BYTE*)::LoadResource(hInst, hResource);

    if (m_hBuffer)
    {
        void* pSource = ::LockResource(::LoadResource(hInst, hResource));

        if (!pSource) return FALSE;

        void* pDest = ::GlobalLock(m_hBuffer);
        if (pDest)
        {
            CopyMemory(pDest, pSource, nImageSize);

            if (::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
            {
                img = Gdiplus::Image::FromStream(pStream);
                pStream->Release();
            }

            ::GlobalUnlock(m_hBuffer);
        }

        ::GlobalFree(m_hBuffer);
        m_hBuffer = NULL;
    }

    if (Gdiplus::Ok != img->GetLastStatus())
    {
        if (img)
        {
            delete img;
            img = NULL;
        }
        return FALSE;
    }

    return TRUE;
}

