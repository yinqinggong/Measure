#pragma once
#include <afxwin.h>
class CMyButton :  public CButton
{
private:
    CBrush m_brushBlack;

public:
    DECLARE_MESSAGE_MAP()
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    virtual void PreSubclassWindow();
    afx_msg void OnBnClicked();
};

