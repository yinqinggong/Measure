#include "pch.h"
#include "MyStatic.h"

IMPLEMENT_DYNAMIC(CMyStatic, CStatic)
CMyStatic::CMyStatic()
{
	m_textColor = RGB(255, 255, 255); // 默认文本颜色为黑色
	m_bkColor = RGB(0, 0, 0); // 默认背景色为白色
}

CMyStatic::~CMyStatic()
{
}

BEGIN_MESSAGE_MAP(CMyStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CMyStatic::SetTextColor(COLORREF color)
{
    m_textColor = color;
    RedrawWindow();
}

void CMyStatic::SetBkColor(COLORREF color)
{
    m_bkColor = color;
    RedrawWindow();
}

void CMyStatic::OnPaint()
{
    CPaintDC dc(this); // 用于绘制的设备上下文

    CRect rect;
    GetClientRect(rect);

    CBrush brush(m_bkColor);
    dc.FillRect(rect, &brush);

    dc.SetTextColor(m_textColor);
    dc.SetBkMode(TRANSPARENT);

    CString text;
    GetWindowText(text);

    //dc.DrawText(text, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    dc.DrawText(text, rect, DT_SINGLELINE | DT_VCENTER);
}

