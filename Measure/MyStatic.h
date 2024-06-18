#pragma once
#include <afxwin.h>
class CMyStatic : public CStatic
{
    DECLARE_DYNAMIC(CMyStatic)
private:
    COLORREF m_textColor;
    COLORREF m_bkColor;
public:
    CMyStatic();
    virtual ~CMyStatic();
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    void SetTextColor(COLORREF color);
    void SetBkColor(COLORREF color);

};

