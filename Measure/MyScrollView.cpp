#include "pch.h"
#include "MyScrollView.h"

IMPLEMENT_DYNCREATE(CMyScrollView, CScrollView)

BEGIN_MESSAGE_MAP(CMyScrollView, CScrollView)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

CMyScrollView::CMyScrollView()
{
    m_columns = 4; // 每行四个子窗口

     // 示例数据数量
    m_totalDataCount = 200;
}

CMyScrollView::~CMyScrollView()
{
   /* for (CWnd* pChild : m_childWindows)
    {
        if (pChild != nullptr)
        {
            pChild->DestroyWindow();
            delete pChild;
        }
    }
    m_childWindows.clear();*/
}

void CMyScrollView::OnInitialUpdate()
{
    CScrollView::OnInitialUpdate();

    // 设置初始滚动大小
    CSize sizeTotal;
    sizeTotal.cx = 1000; // 宽度
    sizeTotal.cy = 2000; // 高度
    SetScrollSizes(MM_TEXT, sizeTotal);

   

    CreateChildWindows(m_totalDataCount);
    LayoutChildWindows();
}

void CMyScrollView::OnDraw(CDC* pDC)
{
    // 在此绘制滚动视图的内容（如果有需要的话）
}

void CMyScrollView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    CScrollView::OnUpdate(pSender, lHint, pHint);
    // 更新内容时重新布局
    LayoutChildWindows();
}

void CMyScrollView::CreateChildWindows(int nCount)
{
    for (int i = 0; i < nCount; ++i)
    {
        CWnd* pChild = new CWnd();
        pChild->Create(NULL, _T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 5000 + i);
        m_childWindows.push_back(pChild);

        // 创建子控件，例如静态文本
        CStatic* pStatic = new CStatic();
        if (pStatic->Create(_T("Child Window"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), pChild))
        {
            CString strText;
            strText.Format(_T("Child Window %d"), i);
            pStatic->SetWindowTextW(strText);
        }
        else
        {
            delete pStatic;
        }
    }
}

void CMyScrollView::LayoutChildWindows()
{
    CRect clientRect;
    GetClientRect(&clientRect);

    int childWidth = clientRect.Width() / m_columns;
    int childHeight = 100; // 每个子窗口的高度
    int x = 0;
    int y = 0;

    for (size_t i = 0; i < m_childWindows.size(); ++i)
    {
        if (i % m_columns == 0)
        {
            x = 0;
            y += childHeight;
        }

        CWnd* pChild = m_childWindows[i];
        pChild->MoveWindow(x, y - childHeight, childWidth, childHeight);

        // 设置子控件的位置
        CStatic* pStatic = static_cast<CStatic*>(pChild->GetWindow(GW_CHILD));
        if (pStatic)
        {
            pStatic->MoveWindow(10, 10, childWidth - 20, childHeight - 20);
        }

        x += childWidth;
    }

    // 更新滚动视图的大小
    CSize sizeTotal;
    sizeTotal.cx = clientRect.Width();
    sizeTotal.cy = y;
    SetScrollSizes(MM_TEXT, sizeTotal);
}

void CMyScrollView::OnDestroy()
{
    CScrollView::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    for (CWnd* pChild : m_childWindows)
    {
        if (pChild != nullptr)
        {
            CStatic* pStatic = static_cast<CStatic*>(pChild->GetWindow(GW_CHILD));
            if (pStatic)
            {
                pStatic->DestroyWindow();
                delete pStatic;
            }
            pChild->DestroyWindow();
            delete pChild;
        }
    }
    m_childWindows.clear();
}
