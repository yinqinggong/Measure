#pragma once


#include "EditWoodWnd.h"
// CWoodEditDlg 对话框

class CWoodEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWoodEditDlg)

public:
	CWoodEditDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CWoodEditDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EDIT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CEditWoodWnd m_editWoodWnd;
public:
	virtual BOOL OnInitDialog();
	void SetWndIndex(int wndIndex)
	{
		m_editWoodWnd.SetWndIndex(wndIndex);
	}
	void SetScaleWood(ScaleWood* pScaleWood)
	{
		m_editWoodWnd.SetScaleWood(pScaleWood);
	}
//	afx_msg void OnClose();
};
