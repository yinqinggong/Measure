#pragma once
#include <afxwin.h>
#include <atlimage.h>
class CMyWnd :  public CWnd
{
protected:
    CImage m_image;
    CPoint m_ptOffset;
    BOOL m_bDragging;
    BOOL m_bDbClick;
    CPoint m_ptLastMousePos;
    BOOL m_bEdit;
    // 声明变量用于记录椭圆的起始点和结束点
    CPoint startPoint, endPoint;

    // 声明变量用于标记是否正在绘制椭圆
    bool isDrawing = false;
public:
    void SetEdit();
public:
    CMyWnd();
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

};

