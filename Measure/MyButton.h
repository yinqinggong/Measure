#pragma once
#include <afxwin.h>

#define WM_USER_MESSAGE_CHECK (WM_USER + 101)

class CMyButton :  public CButton
{
private:
    CBrush m_brushBlack;
    int m_index;

public:
    DECLARE_MESSAGE_MAP()
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    virtual void PreSubclassWindow();
    afx_msg void OnBnClicked();
public:
    void SetDBIndex(int i);
};

