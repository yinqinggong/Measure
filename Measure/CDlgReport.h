﻿#pragma once


// CDlgReport 对话框

class CDlgReport : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgReport)
private:
	BOOL m_inited;
public:
	CDlgReport(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgReport();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REPORT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_sta_len;
	CEdit m_edit_len;
	CStatic m_sta_standard;
	CComboBox m_combo_standard;
	CListCtrl m_list_report;
	CStatic m_sta_num;
	CStatic m_sta_square;
	void InitCtrls();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int InsertListCtrl(CListCtrl& evtListCtrl, CString items[]);
};