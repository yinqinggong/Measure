#pragma once


// CDlgDiameter 对话框

class CDlgDiameter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDiameter)

public:
	CDlgDiameter(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgDiameter();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DIAMETER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	// 用户输入的长直径和短直径的较小者
	float m_diameter;
public:
	float GetDiameter()
	{
		return m_diameter;
	}
};
