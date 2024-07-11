#include "pch.h"
#include "MyImageStatic.h"

IMPLEMENT_DYNAMIC(CMyImageStatic, CStatic)

BEGIN_MESSAGE_MAP(CMyImageStatic, CStatic)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CMyImageStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
    // Forward the click event to the parent
    //Static的直接Parent是CWnd,CWnd的Parent才是CMyScrollView
    GetParent()->GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), STN_CLICKED), (LPARAM)m_hWnd);
}
