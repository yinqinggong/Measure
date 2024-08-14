
// MeasureDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Measure.h"
#include "MeasureDlg.h"
#include "afxdialogex.h"
#include "ScaleAPI.h"
#include "sqlite3.h"
#include "ScaleDB.h"
#include <json/json.h>
#include "common.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "LogFile.h"
#include "xlsxwriter.h"

//excel begin
//#include <afxdisp.h>      // MFC 自动化类库
//#include "CApplication.h" // 自定义的Excel应用程序类
//#include "CWorkbooks.h"   // 工作簿集合类
//#include "CWorkbook.h"    // 工作簿类
//#include "CWorksheets.h"  // 工作表集合类
//#include "CWorksheet.h"   // 工作表类
//#include "CRange.h"       // 单元格范围类
//excel end

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDC_SUB_IMAGE_WND               9000+1
#define IDC_LOGO_IMAGE_STA              9000+2
#define IDC_MIN_IMAGE_STA               9000+3
#define IDC_EXIT_IMAGE_STA              9000+4

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
	//m_pDevice = NULL;
}

CMeasureDlg::~CMeasureDlg()
{
	
}

void CMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideo);
	DDX_Control(pDX, IDC_BTN_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BTN_CROP, m_btnCrop);
	DDX_Control(pDX, IDC_STATIC_BG, m_bgStatic);
	DDX_Control(pDX, IDC_BTN_SCALE, m_btnScale);
	DDX_Control(pDX, IDC_BTN_DATA, m_btnData);
	DDX_Control(pDX, IDC_BTN_DEL, m_btnDel);
	DDX_Control(pDX, IDC_BTN_REPORT, m_btnReport);
	DDX_Control(pDX, IDC_BTN_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BTN_DOWNLOAD, m_btnDownLoad);
}

BEGIN_MESSAGE_MAP(CMeasureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_ADD, &CMeasureDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_CROP, &CMeasureDlg::OnBnClickedBtnCrop)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_DEL, &CMeasureDlg::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_REPORT, &CMeasureDlg::OnBnClickedBtnReport)
	ON_BN_CLICKED(IDC_BTN_SCALE, &CMeasureDlg::OnBnClickedBtnScale)
	ON_BN_CLICKED(IDC_BTN_DATA, &CMeasureDlg::OnBnClickedBtnData)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CMeasureDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DOWNLOAD, &CMeasureDlg::OnBnClickedBtnDownload)
	ON_MESSAGE(WM_USER_MESSAGE, &CMeasureDlg::OnUserMessage)
	ON_CONTROL_RANGE(STN_CLICKED, IDC_MIN_IMAGE_STA, IDC_EXIT_IMAGE_STA, &CMeasureDlg::OnClickStaMinExit)
	ON_WM_CLOSE()
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

	m_btnAdd.MoveWindow(rcWorkArea.Width() * 0.5 - 50 + 150, 5, 100, 30);
	m_btnCrop.MoveWindow(rcWorkArea.Width() * 0.5 - 50 + 300, 5, 100, 30);
	m_staVideo.MoveWindow(30, 50, rcWorkArea.Width() - 60, rcWorkArea.Height() - 100);
	m_staVideo.ShowWindow(SW_HIDE);

	
	//中间黑色背景
	m_brushBG.CreateSolidBrush(RGB(0, 0, 0));//画刷为绿色
	m_bgStatic.MoveWindow(100, 0, rcWorkArea.Width() - 200, rcWorkArea.Height());
	m_bgStatic.ShowWindow(SW_SHOWNORMAL);

	//中间图片区域4056*3040
	RECT rect;
	rect.left = 120;
	rect.top = 20;
	rect.right = rcWorkArea.Width() - 120;
	rect.bottom = rcWorkArea.Height() - 20;
	m_imgWnd.Create(NULL, _T(""), WS_VISIBLE | WS_CHILD, rect, this, IDC_SUB_IMAGE_WND);
	m_imgWnd.ShowWindow(SW_SHOWNORMAL);

	//报表
	m_dlgReport.Create(IDD_DIALOG_REPORT, this);
	m_dlgReport.MoveWindow(&rect);
	m_dlgReport.ShowWindow(SW_HIDE);


	m_dlgData.Create(IDD_DIALOG_DATA, this);
	m_dlgData.MoveWindow(&rect);
	m_dlgData.ShowWindow(SW_HIDE);

	//LOGO
	m_pStaLogo = new CStatic();
	if (m_pStaLogo->Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, CRect(0, 0, 100, 30), this, IDC_LOGO_IMAGE_STA))
	{
		CImage image;
		if (image.Load(GetCurrentPath() + _T("logo.jpg")) == S_OK)
		{
			// 设置静态控件的图片
			m_pStaLogo->SetBitmap((HBITMAP)image.Detach());
		}
		else
		{
			// 图片加载失败的处理
			TRACE(_T("Failed to load image.\n"));
		}
		m_pStaLogo->MoveWindow(0, 0, 100, 30);
	}

	//Min
	m_pStaMin = new CStatic();
	if (m_pStaMin->Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, CRect(0, 0, 100, 30), this, IDC_MIN_IMAGE_STA))
	{
		CImage image;
		if (image.Load(GetCurrentPath() + _T("min.png")) == S_OK)
		{
			// 设置静态控件的图片
			m_pStaMin->SetBitmap((HBITMAP)image.Detach());
		}
		else
		{
			// 图片加载失败的处理
			TRACE(_T("Failed to load image.\n"));
		}
		m_pStaMin->MoveWindow(rcWorkArea.Width() - 100, 0, 39, 26);
	}

	//Exit
	m_pStaExit = new CStatic();
	if (m_pStaExit->Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, CRect(0, 0, 100, 30), this, IDC_EXIT_IMAGE_STA))
	{
		CImage image;
		if (image.Load(GetCurrentPath() + _T("close.png")) == S_OK)
		{
			// 设置静态控件的图片
			m_pStaExit->SetBitmap((HBITMAP)image.Detach());
		}
		else
		{
			// 图片加载失败的处理
			TRACE(_T("Failed to load image.\n"));
		}
		m_pStaExit->MoveWindow(rcWorkArea.Width() - 39, 0, 39, 26);
	}



	//检尺和数据按钮
	m_btnScale.MoveWindow(10, rcWorkArea.Height() * 0.5 - 40, 80, 40);
	m_btnData.MoveWindow(10, rcWorkArea.Height() * 0.5 + 40, 80, 40);

	//右侧五个按钮
	m_btnDel.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5, 80, 40);
	m_btnCrop.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5 - 80, 80, 40);
	m_btnAdd.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5 - 160, 80, 40);
	m_btnReport.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5 + 80, 80, 40);
	m_btnSave.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5 + 160, 80, 40);
	//下载按钮，居中
	m_btnDownLoad.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5, 80, 40);
	m_btnDownLoad.ShowWindow(SW_HIDE);

	////播放直播流视频测试
	//DeviceInfo devInfo = { 0 };
	//devInfo.channel = 0;
	//std::string playUrl = "rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid";
	//CameraInfo cameraInfo = { 0 };
	//strncpy_s(cameraInfo.rtsp, playUrl.c_str(), playUrl.length());
	//devInfo.cameras.push_back(cameraInfo);
	//m_pDevice = new CRtspDevice(devInfo, NULL);
	//HWND wnd = m_staVideo.GetSafeHwnd();

	//if (m_pDevice->StartRealPlay(wnd) <= 0)
	//{
	//	AfxMessageBox(_T("播放失败！"));
	//}
	int ret = create_db();
	if (ret == -1)
	{
		AfxMessageBox(_T("打开数据库失败"));
	}
	/*else if (ret == -2)
	{
		AfxMessageBox(_T("创建数据表失败"));
	}*/
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
		//给窗口设置背景色
		CRect rect;
		CPaintDC dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect, RGB(42, 42, 43));   //控件背景色

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
	/*if (m_pDevice)
	{
		m_pDevice->StopRealPlay();
		delete m_pDevice;
		m_pDevice = NULL;
	}*/
	if (m_pStaLogo)
	{
		delete m_pStaLogo;
		m_pStaLogo = NULL;
	}
	if (m_pStaMin)
	{
		delete m_pStaMin;
		m_pStaMin = NULL;
	}
	if (m_pStaExit)
	{
		delete m_pStaExit;
		m_pStaExit = NULL;
	}
}


//void CMeasureDlg::OnBnClickedBtnCapture()
//{
//	// TODO: 在此添加控件通知处理程序代码
//
//	m_imgWnd.SetStatus(0);
//	m_imgWnd.ShowWindow(SW_SHOW);
//
//	//if (m_pDevice)
//	//{
//	//	m_imgWnd.SetStatus(0);
//	//	m_staVideo.ShowWindow(SW_HIDE);
//	//	m_imgWnd.ShowWindow(SW_SHOW);
//
//	//	m_pDevice->StopRealPlay();
//	//	delete m_pDevice;
//	//	m_pDevice = NULL;
//
//	//	
//	//}
//	//else
//	//{
//	//	m_imgWnd.SetStatus(-1);
//	//	m_staVideo.ShowWindow(SW_SHOW);
//	//	m_imgWnd.ShowWindow(SW_HIDE);
//	//	//播放直播流视频测试
//	//	DeviceInfo devInfo = { 0 };
//	//	devInfo.channel = 0;
//	//	std::string playUrl = "rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid";
//	//	CameraInfo cameraInfo = { 0 };
//	//	strncpy_s(cameraInfo.rtsp, playUrl.c_str(), playUrl.length());
//	//	devInfo.cameras.push_back(cameraInfo);
//	//	m_pDevice = new CRtspDevice(devInfo, NULL);
//	//	HWND wnd = m_staVideo.GetSafeHwnd();
//
//	//	if (m_pDevice->StartRealPlay(wnd) <= 0)
//	//	{
//	//		AfxMessageBox(_T("播放失败！"));
//	//	}
//	//}
//}


void CMeasureDlg::OnBnClickedBtnAdd()
{
	//SetClassLong(this->GetSafeHwnd(), GCLP_HCURSOR, (LONG)LoadCursor(NULL, IDC_CROSS));

	// TODO: 在此添加控件通知处理程序代码
	m_dlgReport.ShowWindow(SW_HIDE);
	m_imgWnd.ShowWindow(SW_SHOWNORMAL);
	if (m_imgWnd.GetStatus() == -1)
	{
		AfxMessageBox(_T("请先截图！"));
		return;
	}
	else if(m_imgWnd.GetStatus() == 0)
	{
		m_imgWnd.SetStatus(1);
	}
	else if (m_imgWnd.GetStatus() == 1)
	{
		m_imgWnd.SetStatus(0);
	}
	else
	{
		m_imgWnd.SetStatus(1);
	}
}


void CMeasureDlg::OnBnClickedBtnCrop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_dlgReport.ShowWindow(SW_HIDE);
	m_imgWnd.ShowWindow(SW_SHOWNORMAL);
	if (m_imgWnd.GetStatus() == -1)
	{
		AfxMessageBox(_T("请先截图！"));
		return;
	}
	else
	{
		m_imgWnd.SetStatus(3);
	}
}

HBRUSH CMeasureDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_BG)
	{
		//pDC->SetBkColor(RGB(0, 255, 0));//背景色为绿色
		//pDC->SetTextColor(RGB(255, 0, 0));//文字为红色
		//pDC->SelectObject(&m_font);//文字为15号字体，华文行楷
		return m_brushBG;
	}
	else if (pWnd->GetDlgCtrlID() == IDC_BTN_SCALE || pWnd->GetDlgCtrlID() == IDC_BTN_DATA)
	{
		//pDC->SetBkColor(RGB(0, 255, 0));//背景色为绿色
		return m_brushBG;
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CMeasureDlg::OnBnClickedBtnDel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_dlgReport.ShowWindow(SW_HIDE);
	m_imgWnd.ShowWindow(SW_SHOWNORMAL);
	if (m_imgWnd.GetStatus() == -1)
	{
		AfxMessageBox(_T("请先截图！"));
		return;
	}
	else if (m_imgWnd.GetStatus() == 2)
	{
		m_imgWnd.SetStatus(0);
	}
	else
	{
	    m_imgWnd.SetStatus(2);
	}
}


void CMeasureDlg::OnBnClickedBtnReport()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_dlgReport.IsWindowVisible())
	{
		m_imgWnd.ShowWindow(SW_SHOWNORMAL);
		m_dlgReport.ShowWindow(SW_HIDE);
	}
	else
	{
		m_imgWnd.ShowWindow(SW_HIDE);
		m_dlgReport.ShowWindow(SW_SHOWNORMAL);
		ScaleWood scaleWood;
		m_imgWnd.GetScaleWood(scaleWood);
		m_dlgReport.SetScaleWood(scaleWood);
	}
}


void CMeasureDlg::OnBnClickedBtnScale()
{
	// TODO: 在此添加控件通知处理程序代码
	m_imgWnd.ShowWindow(SW_SHOWNORMAL);
	m_dlgData.ShowWindow(SW_HIDE);
	m_dlgReport.ShowWindow(SW_HIDE);
	//std::string url;
	//int ret = PostPreview(url);

	//右侧按钮切换显示
	m_btnDel.ShowWindow(SW_SHOWNORMAL);
	m_btnCrop.ShowWindow(SW_SHOWNORMAL);
	m_btnAdd.ShowWindow(SW_SHOWNORMAL);
	m_btnReport.ShowWindow(SW_SHOWNORMAL);
	m_btnSave.ShowWindow(SW_SHOWNORMAL);
	m_btnDownLoad.ShowWindow(SW_HIDE);

	m_imgWnd.ResetCapture();
}

void CMeasureDlg::OnBnClickedBtnData()
{
	// TODO: 在此添加控件通知处理程序代码
	m_imgWnd.ShowWindow(SW_HIDE);
	m_dlgData.ShowWindow(SW_SHOWNORMAL);
	m_dlgData.SetEndTimeCurTime();

	//右侧按钮切换显示
	m_btnDel.ShowWindow(SW_HIDE);
	m_btnCrop.ShowWindow(SW_HIDE);
	m_btnAdd.ShowWindow(SW_HIDE);
	m_btnReport.ShowWindow(SW_HIDE);
	m_btnSave.ShowWindow(SW_HIDE);
	m_btnDownLoad.ShowWindow(SW_SHOWNORMAL);
}
std::string CMeasureDlg::GetStringWoodList(ScaleWood& scaleWood)
{
	// 新建 JSON 对象
	Json::Value root(Json::arrayValue);// 给 JSON 对象添加键值对
	for (size_t i = 0; i < scaleWood.wood_list.size(); i++)
	{
		Json::Value woodAttr;
		woodAttr["diameter"] = scaleWood.wood_list[i].diameter;

		Json::Value diameters;
		diameters["d1"] = scaleWood.wood_list[i].diameters.d1;
		diameters["d2"] = scaleWood.wood_list[i].diameters.d2;
		woodAttr["diameters"] = diameters;

		Json::Value ellipse;
		ellipse["ab1"] = scaleWood.wood_list[i].ellipse.ab1;
		ellipse["ab2"] = scaleWood.wood_list[i].ellipse.ab2;
		ellipse["angel"] = scaleWood.wood_list[i].ellipse.angel;
		ellipse["cx"] = scaleWood.wood_list[i].ellipse.cx;
		ellipse["cy"] = scaleWood.wood_list[i].ellipse.cy;
		ellipse["lx1"] = scaleWood.wood_list[i].ellipse.lx1;
		ellipse["lx2"] = scaleWood.wood_list[i].ellipse.lx2;
		ellipse["ly1"] = scaleWood.wood_list[i].ellipse.ly1;
		ellipse["ly2"] = scaleWood.wood_list[i].ellipse.ly2;
		ellipse["sx1"] = scaleWood.wood_list[i].ellipse.sx1;
		ellipse["sx2"] = scaleWood.wood_list[i].ellipse.sx2;
		ellipse["sy1"] = scaleWood.wood_list[i].ellipse.sy1;
		ellipse["sy2"] = scaleWood.wood_list[i].ellipse.sy2;
		woodAttr["ellipse"] = ellipse;

		root.append(woodAttr);
	}

	Json::StyledWriter writer;
	std::string json_str = writer.write(root);// 将字符串转为 char*const char* data = json_str.c_str();// 打印结果
	return json_str;
}
void CMeasureDlg::OnBnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	m_imgWnd.ShowWindow(SW_SHOWNORMAL);
	m_dlgReport.ShowWindow(SW_HIDE);

	double wood_len = m_dlgReport.GetWoodLen();

	ScaleWood scaleWood = { 0 };
	if (m_imgWnd.GetScaleWood(scaleWood))
	{
		cv::Mat src = cv::imread(GetImagePathUTF8() + std::to_string(scaleWood.id) + ".jpg");
		if (src.data)
		{
			//保存结果图
			cv::Mat r_dst = src.clone(); 
			char text[8] = { 0 };
			for (size_t i = 0; i < scaleWood.wood_list.size(); i++)
			{
				cv::ellipse(r_dst,
					cv::Point(scaleWood.wood_list[i].ellipse.cx, scaleWood.wood_list[i].ellipse.cy),
					cv::Size(scaleWood.wood_list[i].ellipse.ab1, scaleWood.wood_list[i].ellipse.ab2),
					scaleWood.wood_list[i].ellipse.angel, 
					0.0, 360.0, cv::Scalar(0, 255, 0), 2);
				
				sprintf_s(text, 8, "% .2f", scaleWood.wood_list[i].diameter);
				cv::putText(r_dst, text,
					cv::Point(scaleWood.wood_list[i].ellipse.cx - scaleWood.wood_list[i].ellipse.ab1 * 0.7,
						scaleWood.wood_list[i].ellipse.cy/* - scaleWood.wood_list[i].ellipse.ab1 * 0.3*/),
					cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 4);
			}
			cv::imwrite(GetImagePathUTF8() + std::to_string(scaleWood.id) + "_r.jpg", r_dst);

			//保存小图
			cv::Mat s_dst;
			cv::resize(src, s_dst, cv::Size(src.cols/8, src.rows/8));
			cv::imwrite(GetImagePathUTF8() + std::to_string(scaleWood.id) + "_s.jpg", s_dst);
		}
		
		//计算总容积
		double total_v = 0.0;
		double wood_v = 0.0;
		double l = wood_len;
		for (size_t i = 0; i < scaleWood.wood_list.size(); i++)
		{
			int d = scaleWood.wood_list[i].diameter;
			if (d < 14) {
				wood_v = ((0.7854 * l * (d + 0.45 * l + 0.2) * (d + 0.45 * l + 0.2)) / 10000);
			}
			else {
				wood_v = ((0.7854 * l * (d + 0.5 * l + 0.005 * l * l + 0.000125 * l * (14 - l) * (14 - l) * (d - 10)) * (d + 0.5 * l + 0.005 * l * l + 0.000125 * l * (14 - l) * (14 - l) * (d - 10))) / 10000);
			}
			total_v += wood_v;
		}
		
		//获取woodlist串
		std::string woollist = GetStringWoodList(scaleWood);
		int ret = db_query_exist_by_create_time(scaleWood.id);
		if (ret < 0)
		{
			if (ret == -1)
			{
				AfxMessageBox(_T("打开数据库失败"));
				return;
			}
			else
			{
				AfxMessageBox(_T("查询数据失败"));
				return;
			}
		}
		else if (ret > 0)
		{
			ret = db_update_by_create_time(scaleWood.id, scaleWood.wood_list.size(), l, total_v, woollist);
			if (ret == -1)
			{
				AfxMessageBox(_T("打开数据库失败"));
			}
			else if (ret == -2)
			{
				AfxMessageBox(_T("修改数据失败"));
			}
			else
			{
				AfxMessageBox(_T("修改数据成功"));
			}
		}
		else 
		{
			ret = db_insert_record(scaleWood.id, scaleWood.wood_list.size(), l, total_v, woollist);
			if (ret == -1)
			{
				AfxMessageBox(_T("打开数据库失败"));
			}
			else if (ret == -2)
			{
				AfxMessageBox(_T("插入数据失败"));
			}
			else
			{
				AfxMessageBox(_T("保存数据成功"));
			}
		}
		m_imgWnd.ResetCapture();
		//m_imgWnd.ClearScaleWood();
	}
	else
	{
		AfxMessageBox(_T("没有数据保存，请先进行检尺"));
	}

	//db_query_all();

	//return;
	////printf("_Version = %s \n", _Version);
	//std::vector<DB_Data_Row> testVec;
	//char* pcErrMsg = NULL;
	//sqlite3_stmt* pStmt = NULL;
	//sqlite3* pDB = NULL;
	//int nRes = 0;
	//// 格式化SQL语句
	//char cSql[512] = { 0 };
	//// 测试 时间数据
	//char cDBTime[32] = { 0 };
	//unsigned char bBCDTime[7] = { 0 };
	//memcpy(bBCDTime, "\x20\x19\x04\x09\x15\x36\x43", sizeof(bBCDTime));
	//do
	//{
	//	//打开数据库
	//	nRes = sqlite3_open(DB_PATHNAME, &pDB);
	//	if (nRes != SQLITE_OK)
	//	{
	//		//打开数据库失败
	//		// writeLog
	//		printf("sqlite3_open, 打开数据库失败: %s --------------------\n", sqlite3_errmsg(pDB));
	//		break;
	//	}
	//	// 清除 数据库表 test_table
	//	sqlite3_snprintf(512, cSql, "drop table if exists test_table");
	//	sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
	//	if (nRes != SQLITE_OK)
	//	{
	//		printf("清除数据库表test_table 失败: %s --------------------\n", pcErrMsg);
	//		break;
	//	}
	//	printf("Clear test_table successful. \n");
	//	// 创建一个表,如果该表存在，则不创建，并给出提示信息，存储在 zErrMsg 中
	//	sqlite3_snprintf(512, cSql, "CREATE TABLE test_table(\
	//			nID INTEGER PRIMARY KEY,\
	//			cName VARCHAR(50),\
	//			cCreateTime TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),\
	//			ucSeq INTEGER, \
	//			dMoney DOUBLE DEFAULT 15.5 \
	//		);");
	//	nRes = sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
	//	if (nRes != SQLITE_OK)
	//	{
	//		printf("创建数据库表test_table 失败: %s --------------------\n", pcErrMsg);
	//		break;
	//	}
	//	printf("create test_table successful. \n");
	//	// 插入数据
	//	memset(cDBTime, 0x00, sizeof(cDBTime));
	//	_BCDTimeToDBTime(bBCDTime, sizeof(bBCDTime), cDBTime, sizeof(cDBTime));
	//	sqlite3_snprintf(512, cSql, "INSERT INTO test_table(cName, ucSeq) VALUES('当前时间', 8); \
	//			INSERT INTO test_table(cName, cCreateTime, ucSeq, dMoney) VALUES('%s', '%s', %d, %f)", "InputTime", cDBTime, 10, 16.5);
	//	nRes = sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
	//	if (nRes != SQLITE_OK)
	//	{
	//		printf("插入数据库表test_table 失败: %s --------------------\n", pcErrMsg);
	//		break;
	//	}
	//	printf("insert test_table successful. \n");
	//	// 执行操作  "order by cCreateTime ASC"
	//	sqlite3_snprintf(512, cSql, "select * from test_table order by ucSeq DESC");
	//	if (sqlite3_prepare_v2(pDB, cSql, -1, &pStmt, NULL) == SQLITE_OK)
	//	{
	//		// 单步处理返回的每个行结果
	//		while (sqlite3_step(pStmt) == SQLITE_ROW)
	//		{
	//			// 整型数据 处理
	//			DB_Data_Row rowData;
	//			printf("------------------------------\n");
	//			rowData.nID = sqlite3_column_int(pStmt, 0);
	//			printf("rowData.nID = %d\n", rowData.nID);
	//			// 字符串数据 处理
	//			memcpy(rowData.cName, "123456789012345", 16);
	//			strcpy_s(rowData.cName, (const char*)sqlite3_column_text(pStmt, 1));
	//			printf("rowData.cName = %s\n", rowData.cName);
	//			// 验证 strcpy 复制会把'\0' 结束字符也复制过去
	//			for (int idx = 0; idx < 16; idx++)
	//				printf("%c", rowData.cName[idx]);
	//			printf("\n");
	//			// 时间数据 处理
	//			_DBTimeTocTime((char*)sqlite3_column_text(pStmt, 2), (short)sqlite3_column_bytes(pStmt, 2), rowData.cCreateTime);
	//			printf("cCreateTime_len = %d, rowData.cCreateTime = %s\n", strlen(rowData.cCreateTime), rowData.cCreateTime);
	//			memset(cDBTime, 0x00, sizeof(cDBTime));
	//			_cTimeToDBTime(rowData.cCreateTime, strlen(rowData.cCreateTime), cDBTime, sizeof(cDBTime));
	//			printf("cDBTime_len = %d, cDBTime = %s\n", strlen(cDBTime), cDBTime);
	//			// 单字节数据  处理
	//			rowData.ucSeq = sqlite3_column_int(pStmt, 3);
	//			printf("rowData.ucSeq = %d\n", rowData.ucSeq);
	//			// 浮点数据 处理,格式化显示2位小数
	//			rowData.dMoney = sqlite3_column_double(pStmt, 4);
	//			printf("rowData.dMoney = %.2f\n", rowData.dMoney);
	//			testVec.push_back(rowData);
	//		}
	//	}
	//	else
	//	{
	//		printf("sqlite3_prepare_v2, 准备语句失败 : %s --------------------\n", sqlite3_errmsg(pDB));
	//	}
	//	sqlite3_finalize(pStmt);
	//} while (0);
	////关闭数据库
	//sqlite3_close(pDB);
	//pDB = NULL;
	//if (pcErrMsg != NULL)
	//{
	//	sqlite3_free(pcErrMsg); //释放内存
	//	pcErrMsg = NULL;
	//}
}

void CMeasureDlg::GetDownLoadData(std::vector<std::vector<CString>>& wood_data)
{
	std::vector<WoodDBShow> woodDBShowList;
	m_dlgData.GetWoodData(woodDBShowList);

	std::vector<CString> titleVec{ _T("径级"), _T("根数"), _T("长度"), _T("材积") };
	wood_data.push_back(titleVec);

	for (size_t j = 0; j < woodDBShowList.size(); j++)
	{
		std::map<std::string, ReportData> report_map;
		double wood_len = woodDBShowList[j].lenght;
		double total_v = 0.0;
		for (size_t i = 0; i < woodDBShowList[j].scaleWood.wood_list.size(); i++)
		{
			double d = woodDBShowList[j].scaleWood.wood_list[i].diameter;

			std::string str_wood_d = std::to_string(d);
			str_wood_d = str_wood_d.substr(0, str_wood_d.find(".") + 1 + 1);
			auto iter = report_map.find(str_wood_d);
			if (iter != report_map.end())
			{
				iter->second.wood_num++;
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
				double l = wood_len;
				if (d < 14) {
					reportData.wood_v = ((0.7854 * l * (d + 0.45 * l + 0.2) * (d + 0.45 * l + 0.2)) / 10000);
				}
				else {
					reportData.wood_v = ((0.7854 * l * (d + 0.5 * l + 0.005 * l * l + 0.000125 * l * (14 - l) * (14 - l) * (d - 10)) * (d + 0.5 * l + 0.005 * l * l + 0.000125 * l * (14 - l) * (14 - l) * (d - 10))) / 10000);
				}

				report_map.insert(std::make_pair(str_wood_d, reportData));

				total_v += reportData.wood_v;
			}
		}
		CString strTemp;
		for (auto iter = report_map.begin(); iter != report_map.end(); iter++)
		{
			std::vector<CString> contentVec;
			std::string str_wood_d = iter->first;
			UTF8ToUnicode(str_wood_d.c_str(), strTemp);
			contentVec.push_back(strTemp);

			std::string wood_num = std::to_string(iter->second.wood_num);
			UTF8ToUnicode(wood_num.c_str(), strTemp);
			contentVec.push_back(strTemp);

			std::string wood_len = std::to_string(iter->second.wood_l);
			wood_len = wood_len.substr(0, wood_len.find(".") + 1 + 1);
			UTF8ToUnicode(wood_len.c_str(), strTemp);
			contentVec.push_back(strTemp);

			std::string wood_v = std::to_string(iter->second.wood_v);
			wood_v = wood_v.substr(0, wood_v.find(".") + 1 + 3);
			UTF8ToUnicode(wood_v.c_str(), strTemp);
			contentVec.push_back(strTemp);
			wood_data.push_back(contentVec);
		}
	}
}

void CMeasureDlg::GetDownLoadData(std::vector<std::vector<std::string>>& wood_data, int scaleStandard, int& num, double& total_v)
{
	std::vector<WoodDBShow> woodDBShowList;
	m_dlgData.GetWoodData(woodDBShowList);

	std::map<std::string, ReportData, Wood_D_Sort> report_map;
	for (size_t j = 0; j < woodDBShowList.size(); j++)
	{
		num += woodDBShowList[j].amount;
		double wood_len = woodDBShowList[j].lenght;
		for (size_t i = 0; i < woodDBShowList[j].scaleWood.wood_list.size(); i++)
		{
			double d = woodDBShowList[j].scaleWood.wood_list[i].diameter;
			if (scaleStandard == 0)
			{
				//原始d
			}
			else if (scaleStandard == 1)//国标
			{
				d = round(d / 2) * 2;
				d = round(d * 10) / 10;
			}
			else
			{
				//其他进制，三进制 = 0.1 * 3
				d = round(((d - 0.1 * scaleStandard) / 2)) * 2;
				d = d < 0.0 ? 0.0 : d;//应该不会有负值，防止万一
			}

			std::string str_wood_d = std::to_string(d);
			str_wood_d = str_wood_d.substr(0, str_wood_d.find(".") + 1 + 1);
			auto iter = report_map.find(str_wood_d);
			if (iter != report_map.end())
			{
				iter->second.wood_num++;
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
				double l = wood_len;
				if (d < 14) {
					reportData.wood_v = ((0.7854 * l * (d + 0.45 * l + 0.2) * (d + 0.45 * l + 0.2)) / 10000);
				}
				else {
					reportData.wood_v = ((0.7854 * l * (d + 0.5 * l + 0.005 * l * l + 0.000125 * l * (14 - l) * (14 - l) * (d - 10)) * (d + 0.5 * l + 0.005 * l * l + 0.000125 * l * (14 - l) * (14 - l) * (d - 10))) / 10000);
				}

				report_map.insert(std::make_pair(str_wood_d, reportData));

				total_v += reportData.wood_v;
			}
		}
	}

	for (auto iter = report_map.begin(); iter != report_map.end(); iter++)
	{
		std::vector<std::string> contentVec;
		std::string str_wood_d = iter->first;
		contentVec.push_back(str_wood_d);

		std::string wood_num = std::to_string(iter->second.wood_num);
		contentVec.push_back(wood_num);

		std::string wood_len = std::to_string(iter->second.wood_l);
		wood_len = wood_len.substr(0, wood_len.find(".") + 1 + 1);
		contentVec.push_back(wood_len);

		std::string wood_v = std::to_string(iter->second.wood_v);
		wood_v = wood_v.substr(0, wood_v.find(".") + 1 + 3);
		contentVec.push_back(wood_v);
		wood_data.push_back(contentVec);
	}
}

void CMeasureDlg::OnBnClickedBtnDownload1()
{
	std::vector<std::vector<CString>> wood_data;
	GetDownLoadData(wood_data);
	if (wood_data.size() < 2)
	{
		WriteLog(_T("No data saved!"));
		AfxMessageBox(_T("没有数据保存！"));
		return;
	}

	// 假设我们有一个二维数据vector，每个内部vector代表一行
	//std::vector<std::vector<CString>> data = { {_T("hello"), _T("world")},{_T("1234"), _T("5678")},{_T("abcd")} }; // 你的数据源
	// 使用文件对话框让用户选择保存位置和文件名
	std::string tempFileName = GetCurFormatTime();
	CString cstr_tempFileName;
	UTF8ToUnicode(tempFileName.c_str(), cstr_tempFileName);
	cstr_tempFileName += _T(".csv");
	_tsetlocale(LC_CTYPE, _T("chs"));//设置语言环境为中文。
	CFileDialog fileDlg(FALSE, _T("csv"), cstr_tempFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		CString fileName = fileDlg.GetPathName();

		CStdioFile file;
		if (file.Open(fileName, CFile::modeCreate | CFile::modeWrite))
		{
			// 写入标题行（如果有）
			for (size_t i = 0; i < wood_data[0].size(); ++i)
			{
				file.WriteString(wood_data[0][i]);
				file.WriteString(_T(", "));
			}
			file.WriteString(_T("\n"));

			// 写入数据
			for (size_t i = 1; i < wood_data.size(); ++i)
			{
				for (size_t j = 0; j < wood_data[i].size(); ++j)
				{
					file.WriteString(wood_data[i][j]);
					if (j < wood_data[i].size() - 1)
					{
						file.WriteString(_T(", "));
					}
				}
				file.WriteString(_T("\n"));
			}

			file.Close();
			AfxMessageBox(_T("CSV文件导出成功。"));
		}
		else
		{
			AfxMessageBox(_T("无法创建CSV文件。"));
		}
	}
}


std::string CMeasureDlg::GetWorkSheetNameByScaleStandard(int scaleStandard)
{
	std::vector<std::string> standardNames = { 
		"原始径级", "国家标准",
		"二进制", "三进制", 
		"四进制", "五进制", 
		"六进制", "七进制", 
		"八进制", "九进制" };
	if (scaleStandard > 1 && scaleStandard < 10)
	{
		return standardNames[scaleStandard];
	}
	else
	{
		return standardNames[1];
	}
}

void CMeasureDlg::OnBnClickedBtnDownload()
{
	std::vector<std::vector<std::string>> src_wood_data;
	int src_num = 0;
	double src_total_v = 0.0;
	GetDownLoadData(src_wood_data, 0, src_num, src_total_v);

	CString  strIniFile = GetAppdataPath() + _T("config.ini");
	int scaleStandard = GetPrivateProfileInt(APP_NAME_USERINFO, KEY_NAME_STANDARD, 0, strIniFile);
	std::vector<std::vector<std::string>> user_wood_data;
	int user_num = 0;
	double user_total_v = 0.0;
	GetDownLoadData(user_wood_data, scaleStandard < 2 ? 1 : scaleStandard, user_num, user_total_v);

	if (src_wood_data.size() < 1)
	{
		WriteLog(_T("No data saved!"));
		AfxMessageBox(_T("没有数据保存！"));
		return;
	}

	std::string tempFileName = GetCurFormatTime();
	CString cstr_tempFileName;
	UTF8ToUnicode(tempFileName.c_str(), cstr_tempFileName);
	cstr_tempFileName += _T(".xlsx");
	CFileDialog fileDlg(FALSE, _T("xlsx"), cstr_tempFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Excel Files (*.xlsx)|*.xlsx|All Files (*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		CString fileName = fileDlg.GetPathName();
		std::string strName = UnicodeToUtf8(fileName);
		//保存为xlsx
		lxw_workbook* workbook = workbook_new(strName.data());

		lxw_format* format = workbook_add_format(workbook);
		format_set_num_format(format, "0.000");

		lxw_worksheet* worksheet = workbook_add_worksheet(workbook, GBKToUTF8("原始径级").data());
		worksheet_write_string(worksheet, 0, 0, GBKToUTF8("径级").data(), NULL);
		worksheet_write_string(worksheet, 0, 1, GBKToUTF8("根数").data(), NULL);
		worksheet_write_string(worksheet, 0, 2, GBKToUTF8("长度").data(), NULL);
		worksheet_write_string(worksheet, 0, 3, GBKToUTF8("材积").data(), NULL);

		for (size_t i = 0; i < src_wood_data.size(); i++)
		{
			for (size_t j = 0; j < src_wood_data[i].size(); j++)
			{
				//worksheet_write_string(worksheet, i + 1, j, wood_data[i][j].data(), NULL);
				worksheet_write_number(worksheet, i + 1, j, atof(src_wood_data[i][j].data()), NULL);
			}
		}

		worksheet_write_string(worksheet, src_wood_data.size() + 2, 0, GBKToUTF8("总根数：").data(), NULL);
		worksheet_write_number(worksheet, src_wood_data.size() + 2, 1, src_num, NULL);
		worksheet_write_string(worksheet, src_wood_data.size() + 2, 2, GBKToUTF8("总材积：").data(), NULL);
		worksheet_write_number(worksheet, src_wood_data.size() + 2, 3, src_total_v, format);
		
		std::string userWorkSheetName = GetWorkSheetNameByScaleStandard(scaleStandard);
		lxw_worksheet* worksheet2 = workbook_add_worksheet(workbook, GBKToUTF8(userWorkSheetName.data()).data());
		worksheet_write_string(worksheet2, 0, 0, GBKToUTF8("径级").data(), NULL);
		worksheet_write_string(worksheet2, 0, 1, GBKToUTF8("根数").data(), NULL);
		worksheet_write_string(worksheet2, 0, 2, GBKToUTF8("长度").data(), NULL);
		worksheet_write_string(worksheet2, 0, 3, GBKToUTF8("材积").data(), NULL);

		for (size_t i = 0; i < user_wood_data.size(); i++)
		{
			for (size_t j = 0; j < user_wood_data[i].size(); j++)
			{
				//worksheet_write_string(worksheet2, i + 1, j, wood_data[i][j].data(), NULL);
				worksheet_write_number(worksheet2, i + 1, j, atof(user_wood_data[i][j].data()), NULL);
			}
		}

		worksheet_write_string(worksheet2, user_wood_data.size() + 2, 0, GBKToUTF8("总根数：").data(), NULL);
		worksheet_write_number(worksheet2, user_wood_data.size() + 2, 1, user_num, NULL);
		worksheet_write_string(worksheet2, user_wood_data.size() + 2, 2, GBKToUTF8("总材积：").data(), NULL);
		worksheet_write_number(worksheet2, user_wood_data.size() + 2, 3, user_total_v, format);

		workbook_close(workbook);

		AfxMessageBox(_T("xlsx文件导出成功。"));
	}
	else
	{
		AfxMessageBox(_T("无法创建xlsx文件。"));
	}
}

LRESULT CMeasureDlg::OnUserMessage(WPARAM wParam, LPARAM lParam)
{
	ScaleWood * pScaleWood = (ScaleWood*)wParam;
	double* pLenght = (double*)lParam;
	OnBnClickedBtnScale();
	m_imgWnd.ShowHistoryData(pScaleWood);
	m_dlgReport.SetWoodLen(*pLenght);
	delete pScaleWood;
	delete pLenght;
	return 0;
}
void CMeasureDlg::OnClickStaMinExit(UINT nID)
{
	// 处理点击事件
	// nID 是被点击的控件的ID
	/*CString str;
	str.Format(_T("%d"), nID);
	AfxMessageBox(str);*/
	if (nID == IDC_MIN_IMAGE_STA)
	{
		ShowWindow(SW_MINIMIZE);
	}
	else if (nID == IDC_EXIT_IMAGE_STA)
	{
		if (AfxMessageBox(_T("确定退出系统？"), MB_YESNO) == IDNO)
		{
			return;
		}
		PostMessage(WM_CLOSE);
	}
}

void CMeasureDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnClose();
}
