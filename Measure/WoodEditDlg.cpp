// WoodEditDlg.cpp: 实现文件
//

#include "pch.h"
#include "WoodEditDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#define IDC_WOOD_EDIT_WND               8100+4
// CWoodEditDlg 对话框

IMPLEMENT_DYNAMIC(CWoodEditDlg, CDialogEx)

CWoodEditDlg::CWoodEditDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_EDIT, pParent)
{

}

CWoodEditDlg::~CWoodEditDlg()
{
}

void CWoodEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWoodEditDlg, CDialogEx)
//	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CWoodEditDlg 消息处理程序


BOOL CWoodEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow(&rcWorkArea);

	RECT rect;
	GetClientRect(&rect);
	
	m_editWoodWnd.Create(NULL, _T(""), WS_VISIBLE | WS_CHILD, rect, this, IDC_WOOD_EDIT_WND);
	m_editWoodWnd.LoadLocalImage(true);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}