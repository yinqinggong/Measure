#pragma once
#include <afxwin.h>
#include <vector>

class CMyScrollView : public CScrollView
{
protected:
    DECLARE_DYNCREATE(CMyScrollView)

public:
    CMyScrollView();
    ~CMyScrollView();
    virtual void OnInitialUpdate();

protected:
    virtual void OnDraw(CDC* pDC);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

private:
    void LayoutChildWindows();
    void CreateChildWindows(int nCount);

    std::vector<CWnd*> m_childWindows;
    int m_totalDataCount;
    int m_columns;

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
};

