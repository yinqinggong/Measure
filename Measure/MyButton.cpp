#include "pch.h"
#include "MyButton.h"

BEGIN_MESSAGE_MAP(CMyButton, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(BN_CLICKED, &CMyButton::OnBnClicked)
END_MESSAGE_MAP()

HBRUSH CMyButton::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  在此更改 DC 的任何特性
	pDC->SetBkColor(RGB(0, 0, 0)); // 红色背景
	pDC->SetTextColor(RGB(255, 255, 255));
	return m_brushBlack;
	//return (HBRUSH)GetStockObject(NULL_BRUSH);
	// TODO:  如果不应调用父级的处理程序，则返回非 null 画笔
	//return NULL;
}

void CMyButton::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	CButton::PreSubclassWindow();

	ModifyStyle(0, BS_CHECKBOX);  // 确保样式正确
	m_brushBlack.CreateSolidBrush(RGB(0, 0, 0));  // 初始化黑色画刷
}


void CMyButton::OnBnClicked()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxMessageBox(_T("checkbox clicked!"));
}
