// CDlgReport.cpp: 实现文件
//

#include "pch.h"
#include "Measure.h"
#include "CDlgReport.h"
#include "afxdialogex.h"
#include <vector>
#include <iostream>
#include <string>
#include "common.h"

typedef struct typeReportData
{
	float jingji;
	int num;
	float len;
	float chaiji;
}ReportData;

// CDlgReport 对话框

IMPLEMENT_DYNAMIC(CDlgReport, CDialogEx)

CDlgReport::CDlgReport(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REPORT, pParent)
{
	m_inited = FALSE;
}

CDlgReport::~CDlgReport()
{
}

void CDlgReport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LEN, m_sta_len);
	DDX_Control(pDX, IDC_EDIT_LEN, m_edit_len);
	DDX_Control(pDX, IDC_STATIC_STANDARD, m_sta_standard);
	DDX_Control(pDX, IDC_COMBO_STANDARD, m_combo_standard);
	DDX_Control(pDX, IDC_LIST_REPORT, m_list_report);
	DDX_Control(pDX, IDC_STATIC_NUM, m_sta_num);
	DDX_Control(pDX, IDC_STATIC_SQUARE, m_sta_square);
}


BEGIN_MESSAGE_MAP(CDlgReport, CDialogEx)
//	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgReport 消息处理程序


BOOL CDlgReport::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_edit_len.SetWindowTextW(_T("123"));
	m_combo_standard.InsertString(0, _T("原始"));
	m_combo_standard.InsertString(1, _T("国标"));
	m_combo_standard.InsertString(2, _T("广西"));
	m_combo_standard.SetCurSel(0);


	CString strTemp;

	int num = 5;
	std::string strNum = std::to_string(num);
	UTF8ToUnicode(strNum.c_str(), strTemp);
	m_sta_num.SetWindowTextW(_T("根数：") + strTemp);

	float square = 5.3344;
	std::string strSquare = std::to_string(square);
	strSquare = strSquare.substr(0, strSquare.find(".") + 1 + 3);
	UTF8ToUnicode(strSquare.c_str(), strTemp);
	m_sta_square.SetWindowTextW(_T("方数：") + strTemp);

	m_inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//void CDlgReport::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//					   // TODO: 在此处添加消息处理程序代码
//					   // 不为绘图消息调用 CDialogEx::OnPaint()
//
//	InitCtrls();
//}

void CDlgReport::InitCtrls()
{
	RECT rect;
	GetClientRect(&rect);

	int edge = 20;
	int sta_w = 80;
	int edit_w = 150;
	int h = 30;

	m_sta_len.MoveWindow(edge, edge + 5, sta_w * 0.6, h);
	m_edit_len.MoveWindow(edge + sta_w, edge, edit_w, h);
	m_sta_standard.MoveWindow(edge + sta_w + edit_w + edge * 5, edge + 5, sta_w, h);
	m_combo_standard.MoveWindow(edge + sta_w + edit_w + edge * 5 + sta_w, edge, edit_w, h);

	m_sta_num.MoveWindow(edge, rect.bottom - 50, sta_w, h);
	m_sta_square.MoveWindow(edge * 5 + sta_w, rect.bottom - 50, sta_w, h);
	m_list_report.MoveWindow(edge, edge * 2 + h, rect.right - edge * 2, rect.bottom - (edge * 2 + h) * 2 - h);
}


void CDlgReport::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (!m_inited)
	{
		return;
	}
	InitCtrls();

	RECT rect;
	GetClientRect(&rect);

	m_list_report.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list_report.InsertColumn(0, _T("径级"), LVCFMT_LEFT, (rect.right - 20 * 3) * 0.25);
	m_list_report.InsertColumn(1, _T("根数"), LVCFMT_LEFT, (rect.right - 20 * 3) * 0.25);
	m_list_report.InsertColumn(2, _T("长度"), LVCFMT_LEFT, (rect.right - 20 * 3) * 0.25);
	m_list_report.InsertColumn(3, _T("材积"), LVCFMT_LEFT, (rect.right - 20 * 3) * 0.25);


	std::vector<ReportData> dataArray;
	dataArray.push_back({ 1.0, 1, 2.0, 3.0 });
	dataArray.push_back({ 1.0, 1, 2.0, 3.0 });
	dataArray.push_back({ 1.0, 1, 2.0, 3.0 });
	dataArray.push_back({ 1.0, 1, 2.0, 3.0 });
	dataArray.push_back({ 1.0, 1, 2.0, 3.0 });
	dataArray.push_back({ 1.0, 1, 2.0, 3.0 });
	CString items[4];
	for (int i = 0; i < dataArray.size(); i++)
	{
		std::string jingji = std::to_string(dataArray[i].jingji); 
		jingji = jingji.substr(0, jingji.find(".") + 1 + 3);
		UTF8ToUnicode(jingji.c_str(), items[0]);

		std::string num = std::to_string(dataArray[i].num);
		UTF8ToUnicode(num.c_str(), items[1]);

		std::string len = std::to_string(dataArray[i].len);
		len = len.substr(0, len.find(".") + 1 + 3);
		UTF8ToUnicode(len.c_str(), items[2]);

		std::string chaiji = std::to_string(dataArray[i].chaiji);
		chaiji = chaiji.substr(0, chaiji.find(".") + 1 + 3);
		UTF8ToUnicode(chaiji.c_str(), items[3]);

		InsertListCtrl(m_list_report, items);
	}
}


int CDlgReport::InsertListCtrl(CListCtrl& evtListCtrl, CString items[]) 
{
	CString strText;
	int nColumnCount = evtListCtrl.GetHeaderCtrl()->GetItemCount();
	int pos = evtListCtrl.GetItemCount();
	evtListCtrl.InsertItem(LVIF_TEXT | LVIF_STATE, pos, items[0], 0/*LVIS_SELECTED*/, 0 /*LVIS_SELECTED*/, 0, 0);
	for (int j = 1; j < nColumnCount; j++)
	{
		evtListCtrl.SetItemText(pos, j, items[j]);
	}
	return 1;

}