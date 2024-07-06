#pragma once
#include "MyScrollView.h"

// CDlgData 对话框

class CDlgData : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgData)

public:
	CDlgData(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgData();
private:
	CMyScrollView* m_pScrollView;
	BOOL m_inited;
	CBrush m_brushBG;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DATA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CStatic m_sta_start;
	CDateTimeCtrl m_date_start;
	CStatic m_sta_end;
	CDateTimeCtrl m_date_end;
	CButton m_btn_query;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonQuery();
	bool GetScaleWoodFromJsonString(std::string& jsonstr, ScaleWood& scaleWood);
};
