#include "stdafx.h"
#include "UtilWnd.h"
#include <processthreadsapi.h>


bool RectContains(const CRect& rt1, const CRect& rt2)
{
    if (rt1.PtInRect(rt2.TopLeft()) &&
        rt1.PtInRect(rt2.BottomRight()))
        return true;

    return false;
}

CPoint ConvertToPoint(CWnd* wnd_src, CWnd* wnd_dst, const CPoint& pt_src)
{
    CRect win_rt(pt_src, CSize(0, 0));
    wnd_src->ClientToScreen(win_rt);

    CPoint pt_convert = win_rt.TopLeft();
    wnd_dst->ScreenToClient(&pt_convert);

    return pt_convert;
}

void DrawRectangle(CDC* dc, const CRect& rt, COLORREF line_color)
{
    CPen pen(PS_SOLID, 1, line_color);
    CBrush brush;
    brush.CreateStockObject(NULL_BRUSH);

    CPen*   old_pen   = dc->SelectObject(&pen);
    CBrush* old_brush = dc->SelectObject(&brush);

    dc->Rectangle(rt);

    dc->SelectObject(old_pen);
    dc->SelectObject(old_brush);
}

void DrawRectangle(CDC* dc, const CRect& rt, COLORREF line_color, COLORREF inner_color)
{
    CPen pen(PS_SOLID, 1, line_color);
    CBrush brush(inner_color);

    CPen*   old_pen   = dc->SelectObject(&pen);
    CBrush* old_brush = dc->SelectObject(&brush);

    dc->Rectangle(rt);

    dc->SelectObject(old_pen);
    dc->SelectObject(old_brush);
}

COLORREF GetColor(const CBrush& brush)
{
    LOGBRUSH lb;
    brush.GetObject(sizeof(LOGBRUSH), &lb);
    byte r = GetRValue(lb.lbColor);
    byte g = GetGValue(lb.lbColor);
    byte b = GetBValue(lb.lbColor);

    return RGB(r, g, b);
}

// Combo Box
BOOL SetCurSel_ByData(CComboBox& combo_box, int nData)
{
    int nItem = combo_box.GetCount();
    for (int i = 0; i < nItem; i++) {
        DWORD data = (DWORD)combo_box.GetItemData(i);
        if (data == nData) {
            combo_box.SetCurSel(i);
            break;
        }
    }
    return TRUE;
}

BOOL SetCurSel_ByIndex(CComboBox& combo_box, int nIdx)
{
    int nItem = combo_box.GetCount();
    if (nItem == 0) return FALSE;
    if (nIdx < 0 || nIdx >= nItem) {
        combo_box.SetCurSel(0);
    }
    else combo_box.SetCurSel(nIdx);
    return TRUE;
}

BOOL SetCurSel_ByText(CComboBox& combo_box, const CString& strText)
{
    CString strValue;
    int i;
    int nItem = combo_box.GetCount();
    for (i = 0; i < nItem; i++) {
        combo_box.GetLBText(i, strValue);
        if (strValue == strText)
        {
            combo_box.SetCurSel(i);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL GetCurData(CComboBox& combo_box, int& nData)
{
    if (combo_box.GetCount() == 0) return FALSE;
    int nCurSelIdx = combo_box.GetCurSel();
    nData = (int)combo_box.GetItemData(nCurSelIdx);
    return TRUE;
}

BOOL GetCurData(CComboBox& combo_box, CString& strData)
{
    if (combo_box.GetCount() == 0) return FALSE;
    CString* pStringData = (CString*)combo_box.GetItemData(combo_box.GetCurSel());
    if (pStringData) {
        strData = *pStringData;
        return TRUE;
    }
    return FALSE;
}

BOOL AddComboBoxItem(CComboBox& combo_box, const CString& strItemStr, int nData)
{
    int nItemIdx = combo_box.AddString(strItemStr);
    combo_box.SetItemData(nItemIdx, DWORD_PTR(nData));
    return TRUE;
}

// Edit Box
void GetEditText(CEdit& edit, int& nData)
{
    CString strText;
    edit.GetWindowText(strText);
    nData = _ttoi(strText);
}

void GetEditText(CEdit& edit, DWORD& nData)
{
    CString strText;
    edit.GetWindowText(strText);
    nData = _ttoi(strText);
}

void GetEditText(CEdit& edit, CString& value)
{
    CString text;
    GetWindowText(edit.GetSafeHwnd(), text.GetBuffer(256), 256);
    text.ReleaseBuffer();

    value = text.GetString();
}

void GetEditText(CEdit& edit, std::string& value)
{
    CStringA text;
    GetWindowTextA(edit.GetSafeHwnd(), text.GetBuffer(256), 256);
    text.ReleaseBuffer();

    value = text.GetString();
}

void GetEditText(CEdit& edit, std::wstring& value)
{
    CStringW text;
    GetWindowTextW(edit.GetSafeHwnd(), text.GetBuffer(256), 256);
    text.ReleaseBuffer();

    value = text.GetString();
}

void SetEditText(CEdit& edit, const CString& value)
{
    edit.SetWindowText(value);
}

void SetEditText(CEdit& edit, const std::string& value)
{
    SetWindowTextA(edit.GetSafeHwnd(), value.c_str());
}

void SetEditText(CEdit& edit, const std::wstring& value)
{
    SetWindowTextW(edit.GetSafeHwnd(), value.c_str());
}

void SetEditText(CEdit& edit, int nData)
{
    CString strText;
    strText.Format(_T("%d"), nData);
    edit.SetWindowText(strText);
}

void SetFontCtrl(CWnd& wnd, CFont& font, int size)
{
    LOGFONT log_font;
    wnd.GetFont()->GetLogFont(&log_font);
    log_font.lfHeight = size * 10;

    font.DeleteObject();
    font.CreatePointFontIndirect(&log_font);
    wnd.SetFont(&font);
}