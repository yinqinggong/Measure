
// MeasureDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Measure.h"
#include "MeasureDlg.h"
#include "afxdialogex.h"
#include "ScaleAPI.h"
#include "sqlite3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDC_SUB_IMAGE_WND               9000+1

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

	//中间图片区域
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

	//检尺和数据按钮
	m_btnScale.MoveWindow(10, rcWorkArea.Height() * 0.5 - 40, 80, 40);
	m_btnData.MoveWindow(10, rcWorkArea.Height() * 0.5 + 40, 80, 40);

	//右侧五个按钮
	m_btnDel.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5, 80, 40);
	m_btnCrop.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5 - 80, 80, 40);
	m_btnAdd.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5 - 160, 80, 40);
	m_btnReport.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5 + 80, 80, 40);
	m_btnSave.MoveWindow(rcWorkArea.Width() - 90, rcWorkArea.Height() * 0.5 + 160, 80, 40);

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
	if (m_pDevice)
	{
		m_pDevice->StopRealPlay();
		delete m_pDevice;
		m_pDevice = NULL;
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
	m_imgWnd.ShowWindow(SW_HIDE);
	m_dlgReport.ShowWindow(SW_SHOWNORMAL);
}


void CMeasureDlg::OnBnClickedBtnScale()
{
	// TODO: 在此添加控件通知处理程序代码
	m_imgWnd.ShowWindow(SW_SHOWNORMAL);
	m_dlgData.ShowWindow(SW_HIDE);
	//std::string url;
	//int ret = PostPreview(url);
}

void CMeasureDlg::OnBnClickedBtnData()
{
	// TODO: 在此添加控件通知处理程序代码
	m_imgWnd.ShowWindow(SW_HIDE);
	m_dlgData.ShowWindow(SW_SHOWNORMAL);
}

//数据库表test_table中行结构体
typedef struct DB_DataFormat
{
	int	 	nID;
	char 	cName[50];
	char 	cCreateTime[15];    // YYYYMMDDHHMMSS
	unsigned char 	ucSeq;
	double 	dMoney;
}DB_Data_Row, * PDB_Data_Row;
// 20190409153643(Hex) -> "2019-04-09 15:36:43"
void _BCDTimeToDBTime(unsigned char* BCDTime_in, short BCDTime_len, char* DBTime_out, short DBTime_len)
{
	assert(BCDTime_len == 7);

	snprintf(DBTime_out, DBTime_len, "%02X%02X-%02X-%02X %02X:%02X:%02X", BCDTime_in[0], BCDTime_in[1],
		BCDTime_in[2], BCDTime_in[3], BCDTime_in[4], BCDTime_in[5], BCDTime_in[6]);
}

// 20190409153643(char) -> "2019-04-09 15:36:43"
void _cTimeToDBTime(char* cTime_in, short cTime_len, char* DBTime_out, short DBTime_len)
{
	assert(cTime_len == 14);

	snprintf(DBTime_out, DBTime_len, "%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c", cTime_in[0], cTime_in[1],
		cTime_in[2], cTime_in[3], cTime_in[4], cTime_in[5], cTime_in[6], cTime_in[7],
		cTime_in[8], cTime_in[9], cTime_in[10], cTime_in[11], cTime_in[12], cTime_in[13]);
}

// "2019-04-09 15:36:43" -> 20190409153643(char)
void _DBTimeTocTime(char* DBTime_in, short DBTime_len, char* cTime_out)
{
	assert(DBTime_len == 19);

	int i = 0, cTime_len = 0;
	for (i = 0; i < DBTime_len; i++)
	{
		// 只存放数字字符
		if (DBTime_in[i] >= '0' && DBTime_in[i] <= '9')
		{
			cTime_out[cTime_len] = DBTime_in[i];
			cTime_len++;
		}
	}
	cTime_out[cTime_len] = '\0';
}
#define DB_PATHNAME    "./yangxt.db"
void CMeasureDlg::OnBnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	//printf("_Version = %s \n", _Version);
	std::vector<DB_Data_Row> testVec;
	char* pcErrMsg = NULL;
	sqlite3_stmt* pStmt = NULL;
	sqlite3* pDB = NULL;
	int nRes = 0;
	// 格式化SQL语句
	char cSql[512] = { 0 };
	// 测试 时间数据
	char cDBTime[32] = { 0 };
	unsigned char bBCDTime[7] = { 0 };
	memcpy(bBCDTime, "\x20\x19\x04\x09\x15\x36\x43", sizeof(bBCDTime));

	do
	{
		//打开数据库
		nRes = sqlite3_open(DB_PATHNAME, &pDB);
		if (nRes != SQLITE_OK)
		{
			//打开数据库失败
			// writeLog
			printf("sqlite3_open, 打开数据库失败: %s --------------------\n", sqlite3_errmsg(pDB));
			break;
		}

		// 清除 数据库表 test_table
		sqlite3_snprintf(512, cSql, "drop table if exists test_table");
		sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
		if (nRes != SQLITE_OK)
		{
			printf("清除数据库表test_table 失败: %s --------------------\n", pcErrMsg);
			break;
		}
		printf("Clear test_table successful. \n");

		// 创建一个表,如果该表存在，则不创建，并给出提示信息，存储在 zErrMsg 中
		sqlite3_snprintf(512, cSql, "CREATE TABLE test_table(\
				nID INTEGER PRIMARY KEY,\
				cName VARCHAR(50),\
				cCreateTime TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),\
				ucSeq INTEGER, \
				dMoney DOUBLE DEFAULT 15.5 \
			);");
		nRes = sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
		if (nRes != SQLITE_OK)
		{
			printf("创建数据库表test_table 失败: %s --------------------\n", pcErrMsg);
			break;
		}
		printf("create test_table successful. \n");

		// 插入数据
		memset(cDBTime, 0x00, sizeof(cDBTime));
		_BCDTimeToDBTime(bBCDTime, sizeof(bBCDTime), cDBTime, sizeof(cDBTime));
		sqlite3_snprintf(512, cSql, "INSERT INTO test_table(cName, ucSeq) VALUES('当前时间', 8); \
				INSERT INTO test_table(cName, cCreateTime, ucSeq, dMoney) VALUES('%s', '%s', %d, %f)", "InputTime", cDBTime, 10, 16.5);
		nRes = sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
		if (nRes != SQLITE_OK)
		{
			printf("插入数据库表test_table 失败: %s --------------------\n", pcErrMsg);
			break;
		}
		printf("insert test_table successful. \n");

		// 执行操作  "order by cCreateTime ASC"
		sqlite3_snprintf(512, cSql, "select * from test_table order by ucSeq DESC");
		if (sqlite3_prepare_v2(pDB, cSql, -1, &pStmt, NULL) == SQLITE_OK)
		{
			// 单步处理返回的每个行结果
			while (sqlite3_step(pStmt) == SQLITE_ROW)
			{
				// 整型数据 处理
				DB_Data_Row rowData;
				printf("------------------------------\n");
				rowData.nID = sqlite3_column_int(pStmt, 0);
				printf("rowData.nID = %d\n", rowData.nID);

				// 字符串数据 处理
				memcpy(rowData.cName, "123456789012345", 16);
				strcpy_s(rowData.cName, (const char*)sqlite3_column_text(pStmt, 1));
				printf("rowData.cName = %s\n", rowData.cName);
				// 验证 strcpy 复制会把'\0' 结束字符也复制过去
				for (int idx = 0; idx < 16; idx++)
					printf("%c", rowData.cName[idx]);
				printf("\n");

				// 时间数据 处理
				_DBTimeTocTime((char*)sqlite3_column_text(pStmt, 2), (short)sqlite3_column_bytes(pStmt, 2), rowData.cCreateTime);
				printf("cCreateTime_len = %d, rowData.cCreateTime = %s\n", strlen(rowData.cCreateTime), rowData.cCreateTime);
				memset(cDBTime, 0x00, sizeof(cDBTime));
				_cTimeToDBTime(rowData.cCreateTime, strlen(rowData.cCreateTime), cDBTime, sizeof(cDBTime));
				printf("cDBTime_len = %d, cDBTime = %s\n", strlen(cDBTime), cDBTime);

				// 单字节数据  处理
				rowData.ucSeq = sqlite3_column_int(pStmt, 3);
				printf("rowData.ucSeq = %d\n", rowData.ucSeq);

				// 浮点数据 处理,格式化显示2位小数
				rowData.dMoney = sqlite3_column_double(pStmt, 4);
				printf("rowData.dMoney = %.2f\n", rowData.dMoney);

				testVec.push_back(rowData);
			}
		}
		else
		{
			printf("sqlite3_prepare_v2, 准备语句失败 : %s --------------------\n", sqlite3_errmsg(pDB));
		}
		sqlite3_finalize(pStmt);

	} while (0);

	//关闭数据库
	sqlite3_close(pDB);
	pDB = NULL;

	if (pcErrMsg != NULL)
	{
		sqlite3_free(pcErrMsg); //释放内存
		pcErrMsg = NULL;
	}
}
