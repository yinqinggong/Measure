#pragma once


// CDlgDebark 对话框

class CDlgDebark : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDebark)

public:
	CDlgDebark(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgDebark();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DEBARK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnConfirm();
	CButton m_checkDebark;
};
