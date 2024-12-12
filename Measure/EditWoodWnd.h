// MyWnd2.h
#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include <vector>
#include "ScaleAPI.h"
#include "BgColorBtn.h"
#include <gdiplus.h>
using namespace Gdiplus;

#define WM_USER_MESSAGE_FINISHED (WM_USER + 102)

class CEditWoodWnd : public CWnd
{
public:
    CEditWoodWnd();
    virtual ~CEditWoodWnd();

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
    ScaleWood* m_pScaleWood;
    //���ڻ��Ƶ���Բ
    WoodEllipse m_ellipse_add;
    CBgColorBtn m_btnAdd;
    CBgColorBtn m_btnCrop;
    CBgColorBtn m_btnDel;
    CBgColorBtn m_btnSave;
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
    void SetScaleWood(ScaleWood* pScaleWood)
    {
        m_pScaleWood = pScaleWood;
    }
    bool GetScaleWood(ScaleWood* pScaleWood)
    {
        if (m_pScaleWood->id <= 0)
        {
            return false;
        }
        pScaleWood = m_pScaleWood;
        return true;
    }
    int GetScaleWoodID()
    {
        return m_pScaleWood->id;
    }
    /*void ClearScaleWood()
    {
        m_scaleWood = { 0 };
    }*/
    void ResetCapture();
    void ShowHistoryData(ScaleWood* pScaleWood);

    afx_msg void OnPaint();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); 
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnBnClickedBtnAdd();
    afx_msg void OnBnClickedBtnCrop();
    afx_msg void OnBnClickedBtnDel();
    afx_msg void OnBnClickedBtnSave();
    DECLARE_MESSAGE_MAP()
public:
    void AdjustImageOrigin();
    void LimitScalingFactor();

    BOOL LoadLocalImage(bool firstInit);
    BOOL LoadLocalImage(LPCTSTR lpszPath, bool firstInit);
    CPoint ScreenToImage(CPoint screenPoint);
    CPoint ImageToScreen(CPoint imagePoint);
//
//private:
//    CWinThread* m_recThread;
//    HANDLE m_hRecThreadHandle;
//    static UINT RecThread(LPVOID lpParam);
//    CEvent m_evt_beginRecEvent;
//    bool m_bRun;//�߳��˳���ʶ
//    bool m_recing;//�Ƿ�����ʶ����
//public:
//    bool StartThread();
//    bool StopThread();
//    void RecMethod();
//    bool GetRecing()
//    {
//        return m_recing;
//    }
//    void SetRecing(bool bRec)
//    {
//        m_recing = bRec;
//    }
private:
        int m_wndIndex;
public:
    void ResetBtnBgColor()
    {
        m_btnAdd.SetBackgroundColor(RGB(255, 255, 255));
        m_btnCrop.SetBackgroundColor(RGB(255, 255, 255));
        m_btnDel.SetBackgroundColor(RGB(255, 255, 255));
    }
    void SetWndIndex(int wndIndex)
    {
        m_wndIndex = wndIndex;
    }
};


