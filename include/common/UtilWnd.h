#pragma once

///  @author  Lee Jong Oh
///  @brief   윈도우 유틸리티 용으로 사용되는 api 모듬


///////////////////////////////////////////////////////////////////////////////
// Coordinate
///////////////////////////////////////////////////////////////////////////////

// rt1 에 rt2 가 포함되는지 체크한다.
bool RectContains(const CRect& rt1, const CRect& rt2);

// wnd_src 의 pt_src 좌표를 wnd_dst 의 기준 좌표로 변환해서 리턴한다.
CPoint ConvertToPoint(CWnd* wnd_src, CWnd* wnd_dst, const CPoint& pt_src);

///////////////////////////////////////////////////////////////////////////////
// Draw
///////////////////////////////////////////////////////////////////////////////

void DrawRectangle(CDC* dc, const CRect& rt, COLORREF line_color);
void DrawRectangle(CDC* dc, const CRect& rt, COLORREF line_color, COLORREF inner_color);
COLORREF GetColor(const CBrush& brush);

///////////////////////////////////////////////////////////////////////////////
// Utility for Common Controls & Windows
///////////////////////////////////////////////////////////////////////////////

// Combo Box
BOOL SetCurSel_ByData(CComboBox& combo_box, int nData);
BOOL SetCurSel_ByIndex(CComboBox& combo_box, int nIdx);
BOOL SetCurSel_ByText(CComboBox& combo_box, const CString& strText);
BOOL GetCurData(CComboBox& combo_box, int& nData);
BOOL GetCurData(CComboBox& combo_box, CString& strData);
BOOL AddComboBoxItem(CComboBox& combo_box, const CString& strItemStr, int nData);

// Edit Box
void GetEditText(CEdit& edit, int& nData);
void GetEditText(CEdit& edit, DWORD& nData);
void GetEditText(CEdit& edit, CString& value);
void GetEditText(CEdit& edit, std::string& value);
void GetEditText(CEdit& edit, std::wstring& value);

void SetEditText(CEdit& edit, const CString& value);
void SetEditText(CEdit& edit, const std::string& value);
void SetEditText(CEdit& edit, const std::wstring& value);
void SetEditText(CEdit& edit, int nData);

void SetFontCtrl(CWnd& wnd, CFont& font, int size);