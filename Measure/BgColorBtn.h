#pragma once
#include <afxwin.h>
class CBgColorBtn : public CButton
{
public:
    CBgColorBtn();
    virtual ~CBgColorBtn();

protected:
    afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    DECLARE_MESSAGE_MAP()

private:
    CBrush m_brush;
    COLORREF m_bgColor;
public:
    void SetBackgroundColor(COLORREF color);
};

