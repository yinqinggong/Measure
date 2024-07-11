#include "pch.h"
#include "MyScrollView.h"
#include "MyStatic.h"
#include "MyButton.h"
#include "common.h"

IMPLEMENT_DYNCREATE(CMyScrollView, CScrollView)

BEGIN_MESSAGE_MAP(CMyScrollView, CScrollView)
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    //ON_WM_CTLCOLOR()
    ON_CONTROL_RANGE(STN_CLICKED, 20000, 30000 - 1, &CMyScrollView::OnStaticClicked)
END_MESSAGE_MAP()

CMyScrollView::CMyScrollView(): m_brushBlack(RGB(0, 0, 0))
{
    m_columns = 4; // 每行四个子窗口

     // 示例数据数量
    //m_totalDataCount = 123;
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

void CMyScrollView::SetWoodDBShowList(std::vector<WoodDBShow> woodDBShowList)
{
    m_woodDBShowList = woodDBShowList;
    m_totalDataCount = woodDBShowList.size();
    OnInitialUpdate();
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
    ClearAllChildWindows();

    for (int i = 0; i < nCount; ++i)
    {
        CWnd* pChild = new CWnd();
        pChild->Create(NULL, _T(""), WS_CHILD | WS_VISIBLE, CRect(10, 10, 10, 10), this, 10000 + i);
        m_childWindows.push_back(pChild);
        // 1，创建图片子控件
        CMyImageStatic* pImgSta = new CMyImageStatic();
        if (pImgSta->Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, CRect(10, 10, 10, 10), pChild, 20000 + i))
        {
            CString imagePath;
            UTF8ToUnicode(m_woodDBShowList[i].image_path.c_str(), imagePath);
            //CString imagePath = _T("..\\Doc\\wood.jpg"); // 替换为你的图片路径
            //if (rand() % 2)
            //{
            //    imagePath = _T("..\\Doc\\wood1.jpg");
            //}
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
            CString strTime;
            UTF8ToUnicode(m_woodDBShowList[i].timestamp.c_str(), strTime);
            pTimeSta->SetWindowTextW(strTime);
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
            strText.Format(_T("根数：%d"), m_woodDBShowList[i].amount);
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
            strText.Format(_T("总方数：%.3f"), m_woodDBShowList[i].total_v);
            pTotalSta->SetWindowTextW(strText);
        }
        else
        {
            delete pTotalSta;
        }

        // 5，创建复选框子控件
        CMyButton* pCheckBtn = new CMyButton();
        if (pCheckBtn->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(0, 0, 0, 0), pChild, 60000 + i))
        {
            pCheckBtn->SetCheck(m_woodDBShowList[i].checked);
        }
        else
        {
            delete pCheckBtn;
        }
    }
}

void CMyScrollView::ClearAllChildWindows()
{
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
            CMyButton* pCheckBtn = static_cast<CMyButton*>(m_childWindows[i]->GetDlgItem(60000 + i));
            if (pCheckBtn)
            {
                pCheckBtn->DestroyWindow();
                delete pCheckBtn;
            }

            m_childWindows[i]->DestroyWindow();
            delete m_childWindows[i];
        }
    }

    m_childWindows.clear();
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
        CMyButton* pCheckBtn = static_cast<CMyButton*>(pChild->GetDlgItem(60000 + i));
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
}

void CMyScrollView::OnDestroy()
{
    CScrollView::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    ClearAllChildWindows();
}

BOOL CMyScrollView::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
     // 设置背景色，例如白色
    CBrush brush(RGB(0, 0, 0));
    CRect rect;
    pDC->GetClipBox(&rect); // 获取需要更新的区域
    pDC->FillRect(&rect, &brush);
    // 返回TRUE，表示我们处理了背景擦除
    return TRUE;
    //return CScrollView::OnEraseBkgnd(pDC);
}
//
//
//HBRUSH CMyScrollView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//    // 确保我们只处理复选框
//    if (nCtlColor == CTLCOLOR_BTN)
//    {
//        pDC->SetBkColor(RGB(0, 0, 0));  // 设置背景色为黑色
//        pDC->SetTextColor(RGB(255, 255, 255));  // 设置文本颜色为白色
//        return m_brushBlack;  // 返回黑色画刷
//    }
//
//    // 对于其他控件，调用默认处理
//    return CScrollView::OnCtlColor(pDC, pWnd, nCtlColor);
//
//    //HBRUSH hbr = CScrollView::OnCtlColor(pDC, pWnd, nCtlColor);
//
//    //// TODO:  在此更改 DC 的任何特性
//
//    //// TODO:  如果默认的不是所需画笔，则返回另一个画笔
//    //return hbr;
//}
void CMyScrollView::OnStaticClicked(UINT nID)
{
    // 处理点击事件
    // nID 是被点击的控件的ID
    CString str;
    str.Format(_T("%d"), nID);
    AfxMessageBox(str);
    ScaleWood* pScaleWood = new ScaleWood(m_woodDBShowList[nID - 20000].scaleWood);
    ::PostMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_USER_MESSAGE, (WPARAM)pScaleWood, 0);
}