#include "stdafx.h"
#include "PreviewWnd.h"

#include "InnerThread.h"

#ifndef _WINDOWS_
#include <windows.h>
#endif
#include <mutex>

//////////////////////////////////////////////////////////////////////////
// class PreviewWndImpl
//////////////////////////////////////////////////////////////////////////

class PreviewWnd::PreviewWndImpl : public InnerThread
{
private:
    HWND                            m_window = NULL;    
    static std::recursive_mutex     m_mutex;

public:
    PreviewWndImpl()
    {

    }

    ~PreviewWndImpl()
    {

    }

    static void DrawRect(HDC dc, RECT rt, COLORREF color)
    {
        HBRUSH brush = ::CreateSolidBrush(color);
        FillRect(dc, &rt, brush);
        DeleteObject(brush);
    }

    static void OnPaint(HDC dc, RECT rt, COLORREF color)
    {

    }

    static LRESULT CALLBACK WindowProc(HWND window, unsigned int msg, WPARAM wp, LPARAM lp)
    {
        switch (msg)
        {
        case WM_CREATE:
            break;
        case WM_PAINT:            
            break;
        case WM_CLOSE:
            LOGI << "WM_CLOSE";
            break;
        case WM_DESTROY:
            LOGI << "WM_DESTROY";
            PostQuitMessage(0);
            return 0;
        default:
            break;
        }

        return DefWindowProc(window, msg, wp, lp);
    }

    int Initialize()
    {
        SaveThreadName("Preview window");

        WNDCLASSEX wndclass = { sizeof(WNDCLASSEX), CS_DBLCLKS, PreviewWndImpl::WindowProc,
                                0, 0, GetModuleHandle(0), LoadIcon(0,IDI_APPLICATION),
                                LoadCursor(0,IDC_ARROW), HBRUSH(COLOR_WINDOW + 1),
                                0, _T("PreviewWndImpl"), LoadIcon(0,IDI_APPLICATION) };

        if (0 == RegisterClassEx(&wndclass))
            return 1;

        return 0;
    }

    int Finalize()
    {
        return 0;
    }

    int Activate()
    {
        InnerThread::StartThread();

        return 0;
    }

    int Deactivate()
    {
        ::SendMessage(m_window, WM_CLOSE, 0, 0);

        InnerThread::JoinThread();

        return 0;
    }        

    int DrawImageByDibRGB24(byte* data, int width, int height)
    {
        RECT rt;
        GetClientRect(m_window, &rt);

        int client_width  = rt.right - rt.left;
        int client_height = rt.bottom - rt.top;

        HDC dc = GetDC(m_window);
        HDC memdc = CreateCompatibleDC(dc);
        HBITMAP mem_bitmap = CreateCompatibleBitmap(dc, width, height);
        HBITMAP old_bitmap = (HBITMAP)SelectObject(memdc, mem_bitmap);

        BITMAPINFOHEADER bmiHeader;
        memset(&bmiHeader, 0, sizeof(BITMAPINFOHEADER));
        bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmiHeader.biCompression = BI_RGB;
        bmiHeader.biBitCount    = 24;
        bmiHeader.biWidth       = width;
        bmiHeader.biHeight      = -height;
        bmiHeader.biPlanes      = 1;
        SetDIBitsToDevice(memdc,    // 출력 대상 DC
            0, 0,                   // 출력 대상 x, y 좌표
            width, height,          // DIB 원본 사각형 Width, Height
            0, 0,                   // DIB 원본 x, y
            0, height,              // 첫 번째 스캔 라인, 출력할 스캔 라인의 개수
            data, (BITMAPINFO *)&bmiHeader, DIB_RGB_COLORS);

        SetStretchBltMode(dc, COLORONCOLOR);
        SetBkMode(dc, TRANSPARENT);
        StretchBlt(
            dc, 0, 0, client_width, client_height,
            memdc, 0, 0, width, height, SRCCOPY);

        SelectObject(memdc, old_bitmap);
        DeleteObject(mem_bitmap);
        DeleteDC(memdc);

        ReleaseDC(m_window, dc);

        return 0;
    }

    virtual void ThreadLoop() override
    {
        LOGI << "Preview thread loop start...";

        m_window = CreateWindowEx(0, _T("PreviewWndImpl"), _T("Preview window"),
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), 0);

        if (m_window)
        {
            ShowWindow(m_window, SW_SHOWDEFAULT);

            MSG msg;
            while (GetMessage(&msg, 0, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        LOGI << "Preview thread loop end...";
    }
};

std::recursive_mutex PreviewWnd::PreviewWndImpl::m_mutex;

//////////////////////////////////////////////////////////////////////////
// class PreviewWnd
//////////////////////////////////////////////////////////////////////////

PreviewWnd::PreviewWnd()
{
    m_impl = std::make_shared<PreviewWndImpl>();
}

PreviewWnd::~PreviewWnd()
{

}

int PreviewWnd::Initialize()
{
    return m_impl->Initialize();
}

int PreviewWnd::Finalize()
{
    return m_impl->Finalize();
}

int PreviewWnd::Activate()
{
    return m_impl->Activate();
}

int PreviewWnd::Deactivate()
{
    return m_impl->Deactivate();
}

int PreviewWnd::DrawImage(byte* data, int width, int height)
{
    if (nullptr == data || width < 0 || height < 0)
        return 1;
    
    return m_impl->DrawImageByDibRGB24(data, width, height);
}