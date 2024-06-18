// DlgData.cpp: 实现文件
//

#include "pch.h"
#include "Measure.h"
#include "DlgData.h"
#include "afxdialogex.h"


#define IDC_SUB_SCROLL_VIEW             9000+2
// CDlgData 对话框

IMPLEMENT_DYNAMIC(CDlgData, CDialogEx)

CDlgData::CDlgData(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DATA, pParent)
{
	m_pScrollView = NULL;
	m_inited = FALSE;
}

CDlgData::~CDlgData()
{
}

void CDlgData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_START, m_sta_start);
	DDX_Control(pDX, IDC_DATE_START, m_date_start);
	DDX_Control(pDX, IDC_STATIC_END, m_sta_end);
	DDX_Control(pDX, IDC_DATE_END, m_date_end);
	DDX_Control(pDX, IDC_BUTTON_QUERY, m_btn_query);
}


BEGIN_MESSAGE_MAP(CDlgData, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgData 消息处理程序


BOOL CDlgData::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	 // 创建滚动视图
	m_brushBG.CreateSolidBrush(RGB(0, 0, 0));//画刷为绿色
	m_inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgData::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (!m_inited)
	{
		return;
	}
	CRect rect;
	GetClientRect(&rect);
	int edge = 20;
	int w = 100;
	int h = 30;

	m_sta_start.MoveWindow(edge, edge * 1.5, w * 0.7, h);
	m_date_start.MoveWindow(edge + w, edge, w * 1.5, h);
	m_sta_end.MoveWindow(edge * 6 + w * 2, edge * 1.5, w * 0.7, h);
	m_date_end.MoveWindow(edge * 6 + w * 3, edge, w * 1.5, h);
	m_btn_query.MoveWindow(rect.right - w * 2, edge, w, h);
	rect.top = 80;
	m_pScrollView = new CMyScrollView();
	if (!m_pScrollView->Create(NULL, _T(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, this, IDC_SUB_SCROLL_VIEW))
	{
		TRACE0("Failed to create scroll view\n");
		return;
	}
	m_pScrollView->OnInitialUpdate();
}


HBRUSH CDlgData::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_START || 
		pWnd->GetDlgCtrlID() == IDC_STATIC_END ||
		pWnd->GetDlgCtrlID() == IDC_BUTTON_QUERY)
	{
		pDC->SetBkColor(RGB(0, 0, 0));//背景色为绿色
		pDC->SetTextColor(RGB(255, 255, 255));//文字为红色
		//pDC->SelectObject(&m_font);//文字为15号字体，华文行楷
		// 创建并返回背景画刷
		//return CreateSolidBrush(m_bgColor);
		return m_brushBG;
	}
	//else if (pWnd->GetDlgCtrlID() == IDC_SUB_SCROLL_VIEW )
	//{
	//	pDC->SetBkColor(RGB(0, 255, 0));//背景色为绿色
	//	return m_brushBG;
	//}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CDlgData::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	//给窗口设置背景色
	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(0, 0, 0));   //控件背景色
}
