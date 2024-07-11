#pragma once
#include <afxwin.h>

class CMyImageStatic : public CStatic
{
public:
    DECLARE_DYNAMIC(CMyImageStatic)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};

