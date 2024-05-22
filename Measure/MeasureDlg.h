
// MeasureDlg.h: 头文件
//

#pragma once
#include "RtspDevice.h"
#include "MyWnd.h"

// CMeasureDlg 对话框
class CMeasureDlg : public CDialogEx
{
// 构造
public:
	CMeasureDlg(CWnd* pParent = nullptr);	// 标准构造函数

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
	CButton m_captureBtn;
	CButton m_paintBtn;
	CMyWnd m_imgWnd;
	CDevice* m_pDevice;
public:
	afx_msg void OnBnClickedBtnCapture();
	afx_msg void OnBnClickedBtnPaint();
	CButton m_cropBtn;
	afx_msg void OnBnClickedBtnCrop();
};
