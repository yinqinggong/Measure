// CDlgDiameter.cpp: 实现文件
//

#include "pch.h"
#include "Measure.h"
#include "CDlgDiameter.h"
#include "afxdialogex.h"
#include "LogFile.h"

// CDlgDiameter 对话框

IMPLEMENT_DYNAMIC(CDlgDiameter, CDialogEx)

CDlgDiameter::CDlgDiameter(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DIAMETER, pParent)
	, m_diameter(0.0)
{

}

CDlgDiameter::~CDlgDiameter()
{
}

void CDlgDiameter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIAMETER, m_diameter);
	DDV_MinMaxFloat(pDX, m_diameter, 0.0, 1000.0);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_staText);
	DDX_Control(pDX, IDOK, m_btnSure);
}


BEGIN_MESSAGE_MAP(CDlgDiameter, CDialogEx)
END_MESSAGE_MAP()


// CDlgDiameter 消息处理程序


BOOL CDlgDiameter::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
#if (LangEN == 1)
	this->SetWindowTextW(_T("Enter diameter length"));
	m_staText.SetWindowTextW(_T("Length of diameter:"));
	m_btnSure.SetWindowTextW(_T("Confirm"));
#else
	this->SetWindowTextW(_T("输入直径长度"));
	m_staText.SetWindowTextW(_T("直径长度："));
	m_btnSure.SetWindowTextW(_T("确定"));
#endif 


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
