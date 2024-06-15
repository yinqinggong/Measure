#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include <vector>

class Wood
{
public:
    CRect rect;
    double diameter;
    bool isDeleting;
};

class CMyWnd :  public CWnd
{
protected:
    CImage m_image;
    CPoint m_ptOffset;
    BOOL m_bDragging;
    BOOL m_bDbClick;
    CPoint m_ptLastMousePos;

    int m_status; //-1：播放 0：截图 1：新增 2：删除 3：多边形
    std::vector<CPoint> m_points; // 用于存储多边形的顶点
    bool m_isPolygonComplete; // 标志多边形是否绘制完成
    bool isCloseEnough(const CPoint& p1, const CPoint& p2, int threshold);
    bool isPointInEllipse(const CPoint& p);

    // 声明变量用于记录椭圆的起始点和结束点
    CPoint startPoint, endPoint;
    // 声明变量用于标记是否正在绘制椭圆
    bool m_isDrawFinished = false;

    //存需要绘制的椭圆
    std::vector<Wood> m_ellipseRects;
    CButton m_btnCapture;
    CButton m_btnRec;
    CButton m_btnDis;
public:
    void SetStatus(int status)
    {
        m_status = status;
    }
    int GetStatus()
    {
        return m_status;
    }
public:
    CMyWnd();
    ~CMyWnd();
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnBnClickedBtnCapture();
    afx_msg void OnBnClickedBtnRec();
    afx_msg void OnBnClickedBtnDis();
};

