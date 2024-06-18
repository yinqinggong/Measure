#include "pch.h"
#include "MyStatic.h"

IMPLEMENT_DYNAMIC(CMyStatic, CStatic)
CMyStatic::CMyStatic()
{
	m_textColor = RGB(255, 255, 255); // Ĭ���ı���ɫΪ��ɫ
	m_bkColor = RGB(0, 0, 0); // Ĭ�ϱ���ɫΪ��ɫ
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
    CPaintDC dc(this); // ���ڻ��Ƶ��豸������

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

