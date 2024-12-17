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

    int m_status; //-1������ 0����ͼ 1������ 2��ɾ�� 3�������
    std::vector<CPoint> m_points; // ���ڴ洢����εĶ���

    CPoint m_startPoint;
    CPoint m_endPoint;// �����������ڼ�¼��Բ����ʼ��ͽ�����

     // �����������ڱ���Ƿ����ڻ�����Բ
    bool m_isDrawFinished = false;
    //����Ҫ���Ƶ���Բ
    ScaleWood m_scaleWood;
    //���ڻ��Ƶ���Բ
    WoodEllipse m_ellipse_add;
    //CButton m_btnCapture;
    //CButton m_btnRec;
    //CButton m_btnDis;
    //͸���Ĳ���
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
    bool m_bRun;//�߳��˳���ʶ
    int m_workStatus;//0:��ʼ״̬ 1:������ 2�����ս��� 3:ʶ���� 4��ʶ�����
    int m_wndIndex;//��������0-3
    unsigned int m_share_wood_id;//�ĸ�ʶ����һ��ID
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


