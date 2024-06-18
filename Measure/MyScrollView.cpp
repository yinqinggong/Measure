#include "pch.h"
#include "MyScrollView.h"
#include "MyStatic.h"

IMPLEMENT_DYNCREATE(CMyScrollView, CScrollView)

BEGIN_MESSAGE_MAP(CMyScrollView, CScrollView)
    ON_WM_DESTROY()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CMyScrollView::CMyScrollView()
{
    m_columns = 4; // 每行四个子窗口

     // 示例数据数量
    m_totalDataCount = 123;
    m_brushBG.CreateSolidBrush(RGB(42, 42, 43));//画刷为绿色
}

CMyScrollView::~CMyScrollView()
{
  
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
    CRect rect;
    GetClientRect(rect);
    rect.right = m_sizeTotal.cx;
    rect.bottom = m_sizeTotal.cy;
    pDC->FillSolidRect(rect, RGB(0, 0, 0));   //控件背景色
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
        pChild->Create(NULL, _T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 10000 + i);
        m_childWindows.push_back(pChild);
        // 1，创建图片子控件
        CStatic* pImgSta = new CStatic();
        if (pImgSta->Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect(0, 0, 0, 0), pChild, 20000 + i))
        {
            CString imagePath = _T("D:\\SIGAI\\Measure\\Doc\\wood.jpg"); // 替换为你的图片路径
            CImage image;
            if (image.Load(imagePath) == S_OK)
            {
                // 设置静态控件的图片
                pImgSta->SetBitmap((HBITMAP)image.Detach());
            }
            else
            {
                // 图片加载失败的处理
                TRACE(_T("Failed to load image.\n"));
            }
        }
        else
        {
            delete pImgSta;
        }

        // 2，创建时间子控件
        CMyStatic* pTimeSta = new CMyStatic();
        if (pTimeSta->Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), pChild, 30000 + i))
        {
            pTimeSta->SetWindowTextW(_T("2024-06-18 23:04:39")); 
        }
        else
        {
            delete pTimeSta;
        }

        // 3，创建根数子控件
        CMyStatic* pNumSta = new CMyStatic();
        if (pNumSta->Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), pChild, 40000 + i))
        {
            CString strText;
            strText.Format(_T("根数：%d"), i);
            pNumSta->SetWindowTextW(strText);
        }
        else
        {
            delete pNumSta;
        }

        // 4，创建总方数子控件
        CMyStatic* pTotalSta = new CMyStatic();
        if (pTotalSta->Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), pChild, 50000 + i))
        {
            CString strText;
            strText.Format(_T("总方数：%d"), i);
            pTotalSta->SetWindowTextW(strText);
        }
        else
        {
            delete pTotalSta;
        }

        // 5，创建复选框子控件
        CButton* pCheckBtn = new CButton();
        if (pCheckBtn->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(0, 0, 0, 0), pChild, 60000 + i))
        {
            // 设置背景色
            //pCheckBtn->SetColor(CButtonST_Ex::BTNST_COLOR_BK_IN, RGB(0, 0, 0));
            //pCheckBtn->SetColor(CButtonST_Ex::BTNST_COLOR_BK_OUT, RGB(0, 0, 0));
           /* pCheckBtn->SetColor(CButtonST_Ex::BTNST_COLOR_BK_FOCUS, RGB(0, 0, 0));
            pCheckBtn->SetColor(CButtonST_Ex::BTNST_COLOR_FG_OUT, RGB(255, 255, 255));
            pCheckBtn->SetColor(CButtonST_Ex::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
            pCheckBtn->SetColor(CButtonST_Ex::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));*/
        }
        else
        {
            delete pCheckBtn;
        }
    }
}

void CMyScrollView::LayoutChildWindows()
{
    CRect clientRect;
    GetClientRect(&clientRect);

    int childWidth = clientRect.Width() / m_columns;
    int childHeight = childWidth; // 每个子窗口的高度
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

        // 1，设置图片子控件的位置
        CStatic* pImgSta = static_cast<CStatic*>(pChild->GetDlgItem(20000 + i));
        if (pImgSta)
        {
            pImgSta->MoveWindow(10, 10, childWidth - 20, childHeight - 110);
        }

        // 2，设置时间子控件
        CMyStatic* pTimeSta = static_cast<CMyStatic*>(pChild->GetDlgItem(30000 + i));
        if (pTimeSta)
        {
            pTimeSta->MoveWindow(10, childHeight - 90, childWidth - 40, 30);
        }
       
        // 3，设置根数子控件
        CMyStatic* pNumSta = static_cast<CMyStatic*>(pChild->GetDlgItem(40000 + i));
        if (pNumSta)
        {
            pNumSta->MoveWindow(10, childHeight - 90 + 30, childWidth-20, 30);
        }
       
        // 4，设置总方数子控件
        CMyStatic* pTotalSta = static_cast<CMyStatic*>(pChild->GetDlgItem(50000 + i));
        if (pTotalSta)
        {
            pTotalSta->MoveWindow(10, childHeight - 90 + 30 + 30, childWidth - 20, 30);
        }
      
        // 5，创建复选框子控件
        CButton* pCheckBtn = static_cast<CButton*>(pChild->GetDlgItem(60000 + i));
        if (pCheckBtn)
        {
            pCheckBtn->MoveWindow(childWidth - 30, childHeight - 95 , 30, 30);
        }


        x += childWidth;
    }

    // 更新滚动视图的大小
    CSize sizeTotal;
    sizeTotal.cx = clientRect.Width();
    sizeTotal.cy = y;
    SetScrollSizes(MM_TEXT, sizeTotal);
    m_sizeTotal = sizeTotal;
}

void CMyScrollView::OnDestroy()
{
    CScrollView::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    for (size_t i = 0; i < m_childWindows.size(); i++)
    {
        if (m_childWindows[i] != nullptr)
        {
            // 1，图片子控件
            CStatic* pImgSta = static_cast<CStatic*>(m_childWindows[i]->GetDlgItem(20000 + i));
            if (pImgSta)
            {
                pImgSta->DestroyWindow();
                delete pImgSta;
            }
            // 2，设置时间子控件
            CMyStatic* pTimeSta = static_cast<CMyStatic*>(m_childWindows[i]->GetDlgItem(30000 + i));
            if (pTimeSta)
            {
                pTimeSta->DestroyWindow();
                delete pTimeSta;
            }

            // 3，设置根数子控件
            CMyStatic* pNumSta = static_cast<CMyStatic*>(m_childWindows[i]->GetDlgItem(40000 + i));
            if (pNumSta)
            {
                pNumSta->DestroyWindow();
                delete pNumSta;
            }

            // 4，设置总方数子控件
            CMyStatic* pTotalSta = static_cast<CMyStatic*>(m_childWindows[i]->GetDlgItem(50000 + i));
            if (pTotalSta)
            {
                pTotalSta->DestroyWindow();
                delete pTotalSta;
            }

            // 5，创建复选框子控件
            CButton* pCheckBtn = static_cast<CButton*>(m_childWindows[i]->GetDlgItem(60000 + i));
            if (pCheckBtn)
            {
                pCheckBtn->DestroyWindow();
                delete pCheckBtn;
            }

            m_childWindows[i]->DestroyWindow();
            delete m_childWindows[i];
        }
    }
   /* for (CWnd* pChild : m_childWindows)
    {
        if (pChild != nullptr)
        {
            CMyStatic* pStatic = static_cast<CMyStatic*>(pChild->GetWindow(GW_CHILD));
            if (pStatic)
            {
                pStatic->DestroyWindow();
                delete pStatic;
            }
            pChild->DestroyWindow();
            delete pChild;
        }
    }*/
    m_childWindows.clear();
}


//HBRUSH CMyScrollView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//    HBRUSH hbr = CScrollView::OnCtlColor(pDC, pWnd, nCtlColor);
//
//    // TODO:  在此更改 DC 的任何特性
//
//    if (pWnd->GetDlgCtrlID() >= 10000 &&
//        pWnd->GetDlgCtrlID() <= 70000)
//    {
//        pDC->SetBkColor(RGB(42, 42, 43));//背景色为绿色
//        pDC->SetTextColor(RGB(255, 255, 255));//文字为红色
//        //pDC->SelectObject(&m_font);//文字为15号字体，华文行楷
//        // 创建并返回背景画刷
//        //return CreateSolidBrush(m_bgColor);
//        return m_brushBG;
//    }
//    // TODO:  如果默认的不是所需画笔，则返回另一个画笔
//    return hbr;
//}
