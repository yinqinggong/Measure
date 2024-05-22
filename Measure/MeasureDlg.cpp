
// MeasureDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Measure.h"
#include "MeasureDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMeasureDlg 对话框



CMeasureDlg::CMeasureDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MEASURE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pDevice = NULL;
}

void CMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideo);
	DDX_Control(pDX, IDC_BTN_CAPTURE, m_captureBtn);
	DDX_Control(pDX, IDC_BTN_PAINT, m_paintBtn);
	DDX_Control(pDX, IDC_BTN_CROP, m_cropBtn);
}

BEGIN_MESSAGE_MAP(CMeasureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CAPTURE, &CMeasureDlg::OnBnClickedBtnCapture)
	ON_BN_CLICKED(IDC_BTN_PAINT, &CMeasureDlg::OnBnClickedBtnPaint)
	ON_BN_CLICKED(IDC_BTN_CROP, &CMeasureDlg::OnBnClickedBtnCrop)
END_MESSAGE_MAP()


// CMeasureDlg 消息处理程序

BOOL CMeasureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//获取工作区
	CRect rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow(&rcWorkArea);
	m_captureBtn.MoveWindow(rcWorkArea.Width() * 0.5 - 50, 5, 100, 30);
	m_paintBtn.MoveWindow(rcWorkArea.Width() * 0.5 - 50 + 150, 5, 100, 30);
	m_cropBtn.MoveWindow(rcWorkArea.Width() * 0.5 - 50 + 300, 5, 100, 30);
	m_staVideo.MoveWindow(30, 50, rcWorkArea.Width() - 60, rcWorkArea.Height() - 100);
	m_staVideo.ShowWindow(SW_SHOW);

	RECT rect;
	rect.left = 30;
	rect.top = 50;
	rect.right = rcWorkArea.Width() - 60;
	rect.bottom = rcWorkArea.Height() - 100;
	m_imgWnd.Create(NULL, _T(""), WS_VISIBLE | WS_CHILD, rect, this, 0);
	m_imgWnd.ShowWindow(SW_HIDE);

	//播放直播流视频测试
	DeviceInfo devInfo = { 0 };
	devInfo.channel = 0;
	std::string playUrl = "rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid";
	CameraInfo cameraInfo = { 0 };
	strncpy_s(cameraInfo.rtsp, playUrl.c_str(), playUrl.length());
	devInfo.cameras.push_back(cameraInfo);
	m_pDevice = new CRtspDevice(devInfo, NULL);
	HWND wnd = m_staVideo.GetSafeHwnd();

	if (m_pDevice->StartRealPlay(wnd) <= 0)
	{
		AfxMessageBox(_T("播放失败！"));
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMeasureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMeasureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMeasureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMeasureDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (m_pDevice)
	{
		m_pDevice->StopRealPlay();
		delete m_pDevice;
		m_pDevice = NULL;
	}
}


void CMeasureDlg::OnBnClickedBtnCapture()
{
	// TODO: 在此添加控件通知处理程序代码

	m_imgWnd.SetStatus(0);
	if (m_pDevice)
	{
		m_staVideo.ShowWindow(SW_HIDE);
		m_imgWnd.ShowWindow(SW_SHOW);

		m_pDevice->StopRealPlay();
		delete m_pDevice;
		m_pDevice = NULL;

		
	}
	else
	{
		m_staVideo.ShowWindow(SW_SHOW);
		m_imgWnd.ShowWindow(SW_HIDE);
		//播放直播流视频测试
		DeviceInfo devInfo = { 0 };
		devInfo.channel = 0;
		std::string playUrl = "rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid";
		CameraInfo cameraInfo = { 0 };
		strncpy_s(cameraInfo.rtsp, playUrl.c_str(), playUrl.length());
		devInfo.cameras.push_back(cameraInfo);
		m_pDevice = new CRtspDevice(devInfo, NULL);
		HWND wnd = m_staVideo.GetSafeHwnd();

		if (m_pDevice->StartRealPlay(wnd) <= 0)
		{
			AfxMessageBox(_T("播放失败！"));
		}
	}
}


void CMeasureDlg::OnBnClickedBtnPaint()
{
	// TODO: 在此添加控件通知处理程序代码
	m_imgWnd.SetStatus(1);
}


void CMeasureDlg::OnBnClickedBtnCrop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_imgWnd.SetStatus(3);
}
