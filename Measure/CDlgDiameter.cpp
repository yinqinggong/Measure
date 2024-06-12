// CDlgDiameter.cpp: 实现文件
//

#include "pch.h"
#include "Measure.h"
#include "CDlgDiameter.h"
#include "afxdialogex.h"


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
}


BEGIN_MESSAGE_MAP(CDlgDiameter, CDialogEx)
END_MESSAGE_MAP()


// CDlgDiameter 消息处理程序
