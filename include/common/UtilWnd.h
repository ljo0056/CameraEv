#pragma once

///  @author  Lee Jong Oh
///  @brief   ������ ��ƿ��Ƽ ������ ���Ǵ� api ���


///////////////////////////////////////////////////////////////////////////////
// Coordinate
///////////////////////////////////////////////////////////////////////////////

// rt1 �� rt2 �� ���ԵǴ��� üũ�Ѵ�.
bool RectContains(const CRect& rt1, const CRect& rt2);

// wnd_src �� pt_src ��ǥ�� wnd_dst �� ���� ��ǥ�� ��ȯ�ؼ� �����Ѵ�.
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