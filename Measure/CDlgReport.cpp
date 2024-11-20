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

// CDlgReport 对话框

IMPLEMENT_DYNAMIC(CDlgReport, CDialogEx)

CDlgReport::CDlgReport(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REPORT, pParent)
{
	m_inited = FALSE;
	m_scaleStandard = 0;
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
	ON_CBN_SELCHANGE(IDC_COMBO_STANDARD, &CDlgReport::OnCbnSelchangeComboStandard)
	ON_EN_CHANGE(IDC_EDIT_LEN, &CDlgReport::OnEnChangeEditLen)
END_MESSAGE_MAP()


// CDlgReport 消息处理程序


BOOL CDlgReport::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_edit_len.SetWindowTextW(_T("2.6"));
	m_combo_standard.InsertString(0, _T("原始径级"));
	m_combo_standard.InsertString(1, _T("国家标准"));
	m_combo_standard.InsertString(2, _T("二进制"));
	m_combo_standard.InsertString(3, _T("三进制"));
	m_combo_standard.InsertString(4, _T("四进制"));
	m_combo_standard.InsertString(5, _T("五进制"));
	m_combo_standard.InsertString(6, _T("六进制"));
	m_combo_standard.InsertString(7, _T("七进制"));
	m_combo_standard.InsertString(8, _T("八进制"));
	m_combo_standard.InsertString(9, _T("九进制"));

	CString  strIniFile = GetAppdataPath() + _T("config.ini");
	m_scaleStandard = GetPrivateProfileInt(APP_NAME_USERINFO, KEY_NAME_STANDARD, 0, strIniFile);
	if (!(m_scaleStandard >= 0 && m_scaleStandard < 10))
	{
		m_scaleStandard = 0;
	}
	m_combo_standard.SetCurSel(m_scaleStandard);
	UpdateWoodData(m_scaleStandard);

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

void CDlgReport::SetScaleWood(ScaleWood scaleWood)
{
	m_scaleWood = scaleWood;
	if (m_scaleStandard >= 0 && m_scaleStandard < 10)
	{
		UpdateWoodData(m_scaleStandard);
	}
	else
	{
		UpdateWoodData(0);
	}
	
}

void CDlgReport::UpdateWoodData(int sd)
{
	CString str_wood_len;
	m_edit_len.GetWindowTextW(str_wood_len);
	double wood_len = _wtof(str_wood_len);

	double total_v = 0.0;
	m_report_map.clear();
	m_list_report.DeleteAllItems();
	for (size_t i = 0; i < m_scaleWood.wood_list.size(); i++)
	{
		double d = m_scaleWood.wood_list[i].diameter;
		if (sd == 0)
		{
			//原始d
		}
		else if (sd == 1)//国标
		{
			d = round(d / 2) * 2;
			d = round(d * 10) / 10;
		}
		//else if (sd == 2)//广西
		//{
		//	d = round(((d - 0.3) / 2)) * 2;
		//}
		else
		{
			//其他进制，三进制 = 0.1 * 3
			d = round(((d - 0.1 * sd) / 2)) * 2;
			d = d < 0.0 ? 0.0 : d;//应该不会有负值，防止万一
		}
		
		std::string str_wood_d = std::to_string(d);
		str_wood_d = str_wood_d.substr(0, str_wood_d.find(".") + 1 + 1);
		auto iter = m_report_map.find(str_wood_d);
		if (iter != m_report_map.end())
		{
			iter->second.wood_num++;
			//double d = m_scaleWood.wood_list[i].diameter;
			double l = wood_len;
			if (d < 14) {
				iter->second.wood_v = ((0.7854 * l * (d + 0.45 * l + 0.2) * (d + 0.45 * l + 0.2)) / 10000);
			}
			else {
				iter->second.wood_v = ((0.7854 * l * (d + 0.5 * l + 0.005 * l * l + 0.000125 * l * (14 - l) * (14 - l) * (d - 10)) * (d + 0.5 * l + 0.005 * l * l + 0.000125 * l * (14 - l) * (14 - l) * (d - 10))) / 10000);
			}

			total_v += iter->second.wood_v;
			iter->second.wood_v *= iter->second.wood_num;
		}
		else
		{
			ReportData reportData;
			reportData.wood_d = d;
			reportData.wood_l = wood_len;
			reportData.wood_num = 1;

			//double d = m_scaleWood.wood_list[i].diameter;
			double l = wood_len;
			if (d < 14) {
				reportData.wood_v = ((0.7854 * l * (d + 0.45 * l + 0.2) * (d + 0.45 * l + 0.2)) / 10000);
			}
			else {
				reportData.wood_v = ((0.7854 * l * (d + 0.5 * l + 0.005 * l * l + 0.000125 * l * (14 - l) * (14 - l) * (d - 10)) * (d + 0.5 * l + 0.005 * l * l + 0.000125 * l * (14 - l) * (14 - l) * (d - 10))) / 10000);
			}

			m_report_map.insert(std::make_pair(str_wood_d, reportData));

			total_v += reportData.wood_v;
		}
	}

	CString items[4];
	for (auto iter = m_report_map.begin(); iter != m_report_map.end(); iter++)
	{
		std::string str_wood_d = iter->first;
		UTF8ToUnicode(str_wood_d.c_str(), items[0]);

		std::string wood_num = std::to_string(iter->second.wood_num);
		UTF8ToUnicode(wood_num.c_str(), items[1]);

		std::string wood_len = std::to_string(iter->second.wood_l);
		wood_len = wood_len.substr(0, wood_len.find(".") + 1 + 1);
		UTF8ToUnicode(wood_len.c_str(), items[2]);

		std::string wood_v = std::to_string(iter->second.wood_v);
		wood_v = wood_v.substr(0, wood_v.find(".") + 1 + 3);
		UTF8ToUnicode(wood_v.c_str(), items[3]);

		InsertListCtrl(m_list_report, items);
	}

	CString strTemp;
	std::string strNum = std::to_string(m_scaleWood.wood_list.size());
	UTF8ToUnicode(strNum.c_str(), strTemp);
	m_sta_num.SetWindowTextW(_T("根数：") + strTemp);

	std::string strSquare = std::to_string(total_v);
	strSquare = strSquare.substr(0, strSquare.find(".") + 1 + 3);
	UTF8ToUnicode(strSquare.c_str(), strTemp);
	m_sta_square.SetWindowTextW(_T("材积：") + strTemp);
}

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

	m_sta_num.MoveWindow(edge, rect.bottom - 50, sta_w * 2, h);
	m_sta_square.MoveWindow(edge * 6 + sta_w, rect.bottom - 50, sta_w * 2, h);
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

void CDlgReport::OnCbnSelchangeComboStandard()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel = m_combo_standard.GetCurSel();
	if (sel >= 0 && sel < 10)
	{
		m_scaleStandard = sel;
		UpdateWoodData(sel);
		//保存到配置文件中
		CString  strIniFile = GetAppdataPath() + _T("config.ini");
		CString tempStr;
		tempStr.Format(_T("%d"), sel);
		WritePrivateProfileString(APP_NAME_USERINFO, KEY_NAME_STANDARD, tempStr, strIniFile);
	}
}

void CDlgReport::OnEnChangeEditLen()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	OnCbnSelchangeComboStandard();
}

double CDlgReport::GetWoodLen()
{
	CString str_wood_len;
	m_edit_len.GetWindowTextW(str_wood_len);
	return _wtof(str_wood_len);
}

void CDlgReport::SetWoodLen(double wood_len)
{
	CString str_wood_len;
	str_wood_len.Format(_T("%.1f"), wood_len);
	m_edit_len.SetWindowTextW(str_wood_len);
}

BOOL CDlgReport::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//屏蔽ESC键按下
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	//屏蔽Enter键按下
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->wParam)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
