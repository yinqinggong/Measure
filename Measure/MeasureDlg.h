
// MeasureDlg.h: 头文件
//

#pragma once
//#include "RtspDevice.h"
#include "MyWnd.h"
#include "CDlgReport.h"
#include "DlgData.h"

// CMeasureDlg 对话框
class CMeasureDlg : public CDialogEx
{
// 构造
public:
	CMeasureDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CMeasureDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEASURE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
private:
	CStatic m_staVideo;
	CMyWnd m_imgWnd;
	CDlgReport m_dlgReport;
	CDlgData m_dlgData;

	CBrush m_brushBG;
	//CDevice* m_pDevice;
	CButton m_btnDownLoad;
	CStatic* m_pStaLogo;
	CStatic* m_pStaMin;
	CStatic* m_pStaExit;
	CStatic m_bgStatic;
	//左侧俩按钮
	CButton m_btnScale;
	CButton m_btnData;
	//右侧五个按钮
	CButton m_btnAdd;//添加
	CButton m_btnCrop;//裁剪
	CButton m_btnDel;//删除
	CButton m_btnReport;//报表
	CButton m_btnSave;//保存
public:
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnCrop();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnReport();
	afx_msg void OnBnClickedBtnScale();
	afx_msg void OnBnClickedBtnData();
	afx_msg void OnBnClickedBtnSave(); 
	std::string GetStringWoodList(ScaleWood& scaleWood);
	afx_msg void OnBnClickedBtnDownload();
	void OnBnClickedBtnDownload1();
	void GetDownLoadData(std::vector<std::vector<CString>>& wood_data);
	void GetDownLoadData(std::vector<std::vector<std::string>>& wood_data);
	afx_msg LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClickStaMinExit(UINT nID);
	afx_msg void OnClose();
};
