#include "pch.h"
#include "MyButton.h"

BEGIN_MESSAGE_MAP(CMyButton, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(BN_CLICKED, &CMyButton::OnBnClicked)
END_MESSAGE_MAP()

HBRUSH CMyButton::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  �ڴ˸��� DC ���κ�����
	pDC->SetBkColor(RGB(0, 0, 0)); // ��ɫ����
	pDC->SetTextColor(RGB(255, 255, 255));
	return m_brushBlack;
	//return (HBRUSH)GetStockObject(NULL_BRUSH);
	// TODO:  �����Ӧ���ø����Ĵ�������򷵻ط� null ����
	//return NULL;
}

void CMyButton::PreSubclassWindow()
{
	// TODO: �ڴ����ר�ô����/����û���

	CButton::PreSubclassWindow();

	ModifyStyle(0, BS_CHECKBOX);  // ȷ����ʽ��ȷ
	m_brushBlack.CreateSolidBrush(RGB(0, 0, 0));  // ��ʼ����ɫ��ˢ
}


void CMyButton::OnBnClicked()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	AfxMessageBox(_T("checkbox clicked!"));
}
