#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include <vector>
#include "ScaleAPI.h"
#include <gdiplus.h>
using namespace Gdiplus;

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


    CPoint m_startPoint;
    CPoint m_endPoint;// 声明变量用于记录椭圆的起始点和结束点

     // 声明变量用于标记是否正在绘制椭圆
    bool m_isDrawFinished = false;

    //存需要绘制的椭圆
    ScaleWood m_scaleWood;
    //正在绘制的椭圆
    WoodEllipse m_ellipse_add;
    CButton m_btnCapture;
    CButton m_btnRec;
    CButton m_btnDis;

    bool isCloseEnough(const CPoint& p1, const CPoint& p2, int threshold);
    bool isPointInEllipse(const CPoint& p);
    bool IsPointInEllipse(int px, int py, int cx, int cy, int a, int b, double angle);
    bool isPointInEllipse(int px, int py);
    bool IsPointInPolygon(int px, int py, const std::vector<CPoint>& polygon);

    void DrawRotatedEllipse(Gdiplus::Graphics* graphics, WoodAttr& woodAttr);
    void DrawRotatedEllipse(Gdiplus::Graphics* graphics, Gdiplus::Pen& pen, int cx, int cy, int ab1, int ab2, double angle);
    void DrawPolygon(Gdiplus::Graphics* graphics, PointF points[], int numPoints);

    double CalculateAngle(int startX, int startY, int endX, int endY);
   
public:
    void SetStatus(int status)
    {
        m_status = status;
    }
    int GetStatus()
    {
        return m_status;
    }
    bool GetScaleWood(ScaleWood& scaleWood)
    {
        if (m_scaleWood.id <= 0)
        {
            return false;
        }
        scaleWood = m_scaleWood;
        return true;
    }
    void ClearScaleWood()
    {
        m_scaleWood = { 0 };
    }
    void ResetCapture();
    void ShowHistoryData(ScaleWood* pScaleWood);
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

