#pragma once
#include "ScaleAPI.h"
#include <map>

typedef struct typeReportData
{
	double wood_d;
	int wood_num;
	double wood_l;
	double wood_v;
}ReportData;

struct Wood_D_Sort {
	bool operator()(const std::string& lhs, const std::string& rhs) const {
		return atof(lhs.c_str()) < atof(rhs.c_str());
	}
};

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

	std::map<std::string, ReportData, Wood_D_Sort> m_report_map;
	ScaleWood m_scaleWood;
	void SetScaleWood(ScaleWood scaleWood);
	void UpdateWoodData(int sd);
	
	void InitCtrls();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int InsertListCtrl(CListCtrl& evtListCtrl, CString items[]);
	afx_msg void OnCbnSelchangeComboStandard();
	afx_msg void OnEnChangeEditLen();
	double GetWoodLen();
	void SetWoodLen(double wood_len);
};
