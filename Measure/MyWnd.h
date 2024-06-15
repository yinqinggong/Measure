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

    int m_status; //-1������ 0����ͼ 1������ 2��ɾ�� 3�������
    std::vector<CPoint> m_points; // ���ڴ洢����εĶ���
    bool m_isPolygonComplete; // ��־������Ƿ�������
    bool isCloseEnough(const CPoint& p1, const CPoint& p2, int threshold);
    bool isPointInEllipse(const CPoint& p);

    // �����������ڼ�¼��Բ����ʼ��ͽ�����
    CPoint startPoint, endPoint;
    // �����������ڱ���Ƿ����ڻ�����Բ
    bool m_isDrawFinished = false;

    //����Ҫ���Ƶ���Բ
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

