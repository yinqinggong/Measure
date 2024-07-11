#pragma once
#include <afxwin.h>
#include <vector>
#include "ScaleAPI.h"
#include "MyImageStatic.h"

#define WM_USER_MESSAGE (WM_USER + 100)

typedef struct defWoodDBShow
{
    std::string image_path;
    std::string timestamp;
    int amount;
    double total_v;
    bool checked;
    ScaleWood scaleWood;
}WoodDBShow;

class CMyScrollView : public CScrollView
{
protected:
    DECLARE_DYNCREATE(CMyScrollView)

public:
    CMyScrollView();
    virtual ~CMyScrollView();
    virtual void OnInitialUpdate();
    void SetWoodDBShowList(std::vector<WoodDBShow> woodDBShowList);

protected:
    virtual void OnDraw(CDC* pDC);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    afx_msg void OnDestroy();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnStaticClicked(UINT nID);
private:
    void LayoutChildWindows();
    void CreateChildWindows(int nCount);
    void ClearAllChildWindows();

    std::vector<CWnd*> m_childWindows;
    int m_totalDataCount;
    int m_columns;
    CBrush m_brushBlack;  // 添加用于背景色的画刷
    std::vector<WoodDBShow> m_woodDBShowList;
    DECLARE_MESSAGE_MAP()

};

