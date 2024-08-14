#include "pch.h"
#include "BgColorBtn.h"
BEGIN_MESSAGE_MAP(CBgColorBtn, CButton)
    ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()

CBgColorBtn::CBgColorBtn() : m_bgColor(RGB(255, 255, 255)) // 默认背景色为白色
{
    m_brush.CreateSolidBrush(m_bgColor);
}

CBgColorBtn::~CBgColorBtn()
{
}
void CBgColorBtn::SetBackgroundColor(COLORREF color)
{
    m_bgColor = color;
    if (m_brush.GetSafeHandle())
    {
        m_brush.DeleteObject();
    }
    m_brush.CreateSolidBrush(m_bgColor);
    Invalidate(); // 重新绘制按钮
}

void CBgColorBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect = lpDrawItemStruct->rcItem;

    // 绘制按钮背景色
    pDC->FillRect(&rect, &m_brush);

    // 如果按钮被按下
    if (lpDrawItemStruct->itemState & ODS_SELECTED)
    {
        pDC->DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);
    }
    else
    {
        pDC->DrawEdge(&rect, EDGE_RAISED, BF_RECT);
    }

    // 绘制按钮文本
    CString strText;
    GetWindowText(strText);
    pDC->SetBkMode(TRANSPARENT);
    pDC->DrawText(strText, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}