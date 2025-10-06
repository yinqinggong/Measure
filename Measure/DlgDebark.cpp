// DlgDebark.cpp: 实现文件
//

#include "pch.h"
#include "DlgDebark.h"
#include "afxdialogex.h"
#include "resource.h"
#include "LogFile.h"
#include "common.h"

// CDlgDebark 对话框

IMPLEMENT_DYNAMIC(CDlgDebark, CDialogEx)

CDlgDebark::CDlgDebark(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DEBARK, pParent)
{

}

CDlgDebark::~CDlgDebark()
{
}

void CDlgDebark::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_DEBARK, m_checkDebark);
}


BEGIN_MESSAGE_MAP(CDlgDebark, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CONFIRM, &CDlgDebark::OnBnClickedBtnConfirm)
END_MESSAGE_MAP()


// CDlgDebark 消息处理程序


BOOL CDlgDebark::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CString  strIniFile = GetAppdataPath() + _T("config.ini");
	int debark = GetPrivateProfileInt(APP_NAME_USERINFO, KEY_NAME_DEBARK, -1, strIniFile);
	if (debark == -1)
	{
		//默认去皮
		WritePrivateProfileString(APP_NAME_USERINFO, KEY_NAME_DEBARK, _T("1"), strIniFile);
		debark = 1;
	}
	m_checkDebark.SetCheck(debark);

#if (LangEN == 1)
	this->SetWindowTextW(_T("Peeled or not"));
	//m_staText.SetWindowTextW(_T("Length of diameter:"));
	//m_btnSure.SetWindowTextW(_T("Confirm"));
#else
	this->SetWindowTextW(_T("是否去皮"));
	//m_staText.SetWindowTextW(_T("直径长度："));
	//m_btnSure.SetWindowTextW(_T("确定"));
#endif 

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgDebark::OnBnClickedBtnConfirm()
{
	//保存到配置文件中
	int debark = m_checkDebark.GetCheck();
	CString tempStr;
	tempStr.Format(_T("%d"), debark);
	CString  strIniFile = GetAppdataPath() + _T("config.ini");
	WritePrivateProfileString(APP_NAME_USERINFO, KEY_NAME_DEBARK, tempStr, strIniFile);
	OnOK();
}
