#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include <vector>

class CMyWnd :  public CWnd
{
protected:
    CImage m_image;
    CPoint m_ptOffset;
    BOOL m_bDragging;
    BOOL m_bDbClick;
    CPoint m_ptLastMousePos;
    BOOL m_bEdit;
    // �����������ڼ�¼��Բ����ʼ��ͽ�����
    CPoint startPoint, endPoint;
    // �����������ڱ���Ƿ����ڻ�����Բ
    bool m_isDrawFinished = false;

    //����Ҫ���Ƶ���Բ
    std::vector<CRect> m_ellipseRects;
public:
    void SetEdit();
public:
    CMyWnd();
    ~CMyWnd();
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

};

