#pragma once
#include <afxwin.h>
#include <vector>

class CMyScrollView : public CScrollView
{
protected:
    DECLARE_DYNCREATE(CMyScrollView)

public:
    CMyScrollView();
    virtual ~CMyScrollView();
    virtual void OnInitialUpdate();

protected:
    virtual void OnDraw(CDC* pDC);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    afx_msg void OnDestroy();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

private:
    void LayoutChildWindows();
    void CreateChildWindows(int nCount);

    std::vector<CWnd*> m_childWindows;
    int m_totalDataCount;
    int m_columns;
    CBrush m_brushBlack;  // 添加用于背景色的画刷
    DECLARE_MESSAGE_MAP()

};

