// CDlgDiameter.cpp: 实现文件
//

#include "pch.h"
#include "Measure.h"
#include "CDlgDiameter.h"
#include "afxdialogex.h"
#include "LogFile.h"
#include "common.h"

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
float CDlgDiameter::GetDiameter()
{
    /* 去皮公式
    径级4-8㎝，减去0.1
    径级8-10㎝，减去0.2
    径级10-16㎝，减去0.4
    径级16cm-20cm，减去0.5
    径级20cm及以上，减去0.6
    */
    CString  strIniFile = GetAppdataPath() + _T("config.ini");
    int debarked = GetPrivateProfileInt(APP_NAME_USERINFO, KEY_NAME_DEBARK, -1, strIniFile);
   
    float fDebark_4_8 = 0.1;
    float fDebark_8_10 = 0.2;
    float fDebark_10_16 = 0.4;
    float fDebark_16_20 = 0.5;
    float fDebark_20 = 0.6;
    if (debarked == 1)
    {
        char debark_4_8[MAX_PATH] = { 0 };
        char debark_8_10[MAX_PATH] = { 0 };
        char debark_10_16[MAX_PATH] = { 0 };
        char debark_16_20[MAX_PATH] = { 0 };
        char debark_20[MAX_PATH] = { 0 };
        char iniPath[MAX_PATH] = { 0 };
        snprintf(iniPath, sizeof(iniPath), "%sconfig.ini", GetAppdataPathUTF8().c_str());

        GetPrivateProfileStringA("DebarkInfo", "debark_4_8", "", debark_4_8, MAX_PATH, iniPath);
        if (strlen(debark_4_8) > 0)
        {
            fDebark_4_8 = atof(debark_4_8);
        }
        GetPrivateProfileStringA("DebarkInfo", "debark_8_10", "", debark_8_10, MAX_PATH, iniPath);
        if (strlen(debark_8_10) > 0)
        {
            fDebark_8_10 = atof(debark_8_10);
        }
        GetPrivateProfileStringA("DebarkInfo", "debark_10_16", "", debark_10_16, MAX_PATH, iniPath);
        if (strlen(debark_10_16) > 0)
        {
            fDebark_10_16 = atof(debark_10_16);
        }
        GetPrivateProfileStringA("DebarkInfo", "debark_16_20", "", debark_16_20, MAX_PATH, iniPath);
        if (strlen(debark_16_20) > 0)
        {
            fDebark_16_20 = atof(debark_16_20);
        }
        GetPrivateProfileStringA("DebarkInfo", "debark_20", "", debark_20, MAX_PATH, iniPath);
        if (strlen(debark_20) > 0)
        {
            fDebark_20 = atof(debark_20);
        }
    }

    if (debarked == 1)
    {
        if (m_diameter >= 4.0 && m_diameter < 8.0)
        {
            m_diameter -= fDebark_4_8;
        }
        else if (m_diameter >= 8.0 && m_diameter < 10.0)
        {
            m_diameter -= fDebark_8_10;
        }
        else if (m_diameter >= 10.0 && m_diameter < 16.0)
        {
            m_diameter -= fDebark_10_16;
        }
        else if (m_diameter >= 16.0 && m_diameter < 20.0)
        {
            m_diameter -= fDebark_16_20;
        }
        else if (m_diameter >= 20.0)
        {
            m_diameter -= fDebark_20;
        }
        else
        {

        }
    }
    return m_diameter;
}
