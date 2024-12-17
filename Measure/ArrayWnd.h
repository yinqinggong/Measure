// MyWnd2.h
#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include <vector>
#include "ScaleAPI.h"
#include <gdiplus.h>
using namespace Gdiplus;

#define WM_USER_MESSAGE_FINISHED (WM_USER + 102)

class CArrayWnd : public CWnd
{
public:
    CArrayWnd();
    virtual ~CArrayWnd();

protected:
    CImage m_image;
    float m_scaleFactor;
    CPoint m_imageOrigin;
    CPoint m_lastMousePos;
    bool m_dragging;

    int m_status; //-1：播放 0：截图 1：新增 2：删除 3：多边形
    std::vector<CPoint> m_points; // 用于存储多边形的顶点

    CPoint m_startPoint;
    CPoint m_endPoint;// 声明变量用于记录椭圆的起始点和结束点

     // 声明变量用于标记是否正在绘制椭圆
    bool m_isDrawFinished = false;
    //存需要绘制的椭圆
    ScaleWood m_scaleWood;
    //正在绘制的椭圆
    WoodEllipse m_ellipse_add;
    //CButton m_btnCapture;
    //CButton m_btnRec;
    //CButton m_btnDis;
    //透传的参数
    std::string m_limg;
    std::string m_rimg;
    std::string m_camparam;

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
    int GetScaleWoodID()
    {
        return m_scaleWood.id;
    }
    void ClearScaleWood()
    {
        m_scaleWood = { 0 };
    }
    void ResetCapture();
    void ShowHistoryData(ScaleWood* pScaleWood);

    afx_msg void OnPaint();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); 
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //afx_msg void OnBnClickedBtnCapture();
    //afx_msg void OnBnClickedBtnRec();
    //afx_msg void OnBnClickedBtnDis();
    DECLARE_MESSAGE_MAP()
private:
    void AdjustImageOrigin();
    void LimitScalingFactor();

    BOOL LoadLocalImage(LPCTSTR lpszPath, bool firstInit);
    CPoint ScreenToImage(CPoint screenPoint);
    CPoint ImageToScreen(CPoint imagePoint);

private:
    CWinThread* m_recThread;
    HANDLE m_hRecThreadHandle;
    static UINT RecThread(LPVOID lpParam);
    CEvent m_evt_beginRecEvent;
    bool m_bRun;//线程退出标识
    int m_workStatus;//0:初始状态 1:拍照中 2：拍照结束 3:识别中 4：识别结束
    int m_wndIndex;//窗口索引0-3
    unsigned int m_share_wood_id;//四个识别共用一个ID
public:
    bool StartThread();
    bool StopThread();
    void PhotoMethod();
    void RecMethod();
    int GetWorkStatus()
    {
        return m_workStatus;
    }
    void SetWorkStatus(int workStatus)
    {
        m_workStatus = workStatus;
    }
    void SetWorkEvent()
    {
        m_evt_beginRecEvent.SetEvent();
    }
    void SetWndIndex(int wndIndex)
    {
        m_wndIndex = wndIndex;
    }
    int GetWndIndex()
    {
        return m_wndIndex;
    }
    void SetShareWoodId(int share_wood_id)
    {
        m_share_wood_id = share_wood_id;
    }
    unsigned int GetShareWoodId()
    {
        return m_share_wood_id;
    }
};


