// MyWnd2.h
#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include <ScaleAPI.h>

class CMyWnd2 : public CWnd
{
public:
    CMyWnd2();
    virtual ~CMyWnd2();

protected:
    CImage m_image;
    float m_scaleFactor;
    CPoint m_imageOrigin;
    CPoint m_lastMousePos;
    bool m_dragging;

    afx_msg void OnPaint();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);

    DECLARE_MESSAGE_MAP()
private:
    void AdjustImageOrigin();
    void LimitScalingFactor();
public:
    BOOL LoadImage(LPCTSTR lpszPath);
    void ResetCapture(){}
    bool GetScaleWood(ScaleWood& scaleWood) { return false; }
    void SetStatus(int status) {}
    int GetStatus()
    {
        return 0;
    }
    int GetScaleWoodID()
    {
        return 1;
    }
    void ShowHistoryData(ScaleWood* pScaleWood){}
};


