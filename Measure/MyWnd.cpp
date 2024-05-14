#include "pch.h"
#include "MyWnd.h"

BEGIN_MESSAGE_MAP(CMyWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

CMyWnd::CMyWnd()
{
	m_bDragging = FALSE;
    m_bDbClick = FALSE;
    m_bEdit = FALSE;
	// 加载图片（将"path_to_your_image"替换为你的图片路径）
	m_image.Load(_T("..\\Doc\\test.jpg")); // 将"path_to_your_image"替换为你的图片路径
	 //m_image.Load(_T("C:\\Users\\yinqi\\Desktop\\13.jpg")); // 将"path_to_your_image"替换为你的图片路径
}

int CMyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

void CMyWnd::OnPaint()
{
	CPaintDC dc(this);

	if (!m_image.IsNull())
	{
        CRect rect;
        GetClientRect(&rect);
        if (!m_bEdit)
        {
            if (/*m_bDragging && */m_bDbClick)
            {
                m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
            }
            else
            {
                //m_image.Draw(dc, 0, 0);
                m_image.Draw(dc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_image.GetWidth(), m_image.GetHeight());
            }
            // 获取与图像关联的设备上下文
            CDC* pDC = CDC::FromHandle(m_image.GetDC());
            if (pDC)
            {
                // 创建多个颜色的画笔
                COLORREF colors[5] = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 0), RGB(255, 0, 255) };

                // 创建多个椭圆的位置和大小
                CRect rectEllipses[5] = {
                    CRect(50, 50, 150, 100),
                    CRect(200, 50, 300, 100),
                    CRect(100, 150, 200, 200),
                    CRect(250, 150, 350, 200),
                    CRect(150, 250, 250, 300)
                };

                // 绘制多个不填充的椭圆曲线
                for (int i = 0; i < 5; ++i)
                {
                    // 创建画笔
                    CPen pen(PS_SOLID, 2, colors[i]); // 2为边框的宽度，可以根据需要调整

                    // 选择画笔
                    CPen* pOldPen = pDC->SelectObject(&pen);

                    // 创建一个空画刷
                    CBrush brush;
                    brush.CreateStockObject(NULL_BRUSH);

                    // 选择空画刷
                    CBrush* pOldBrush = pDC->SelectObject(&brush);

                    // 绘制不填充的椭圆曲线
                    pDC->Ellipse(rectEllipses[i]);

                    // 获取椭圆的中心点和长轴的两个端点
                    CPoint center(rectEllipses[i].CenterPoint());
                    CPoint end1(center.x + (rectEllipses[i].Width() / 2), center.y);
                    CPoint end2(center.x - (rectEllipses[i].Width() / 2), center.y);

                    // 计算长轴的长度
                    double distance = sqrt(pow(end1.x - end2.x, 2) + pow(end1.y - end2.y, 2));

                    // 将长轴的长度绘制在椭圆中心点附近
                    CString strDistance;
                    strDistance.Format(_T("%.2f"), distance);
                    // 设置文本背景模式为透明
                    pDC->SetBkMode(TRANSPARENT);
                    pDC->TextOut(center.x - 20, center.y - 10, strDistance);

                    // 恢复原来的画笔和画刷
                    pDC->SelectObject(pOldPen);
                    pDC->SelectObject(pOldBrush);
                }

                // 释放设备上下文
                m_image.ReleaseDC();
            }
        }
        else 
        {
            m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
            // 创建画笔
            CPen pen(PS_SOLID, 2, RGB(255, 255, 255)); // 2为边框的宽度，可以根据需要调整

            // 选择画笔
            CPen* pOldPen = dc.SelectObject(&pen);

            // 创建一个空画刷
            CBrush brush;
            brush.CreateStockObject(NULL_BRUSH);

            // 选择空画刷
            CBrush* pOldBrush = dc.SelectObject(&brush);

            // 计算椭圆的位置和大小
            CRect rectEllipse(startPoint, endPoint);
            // 擦除之前绘制的椭圆
            //dc.FillRect(CRect(0, 0, 10000, 10000), CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));

            // 绘制椭圆
            dc.Ellipse(rectEllipse);

            // 恢复原来的画笔和画刷
            dc.SelectObject(pOldPen);
            dc.SelectObject(pOldBrush);
            
        }
        
       
    }
    else
    {
        MessageBox(_T("截图失败！"));
    }
}

void CMyWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_image.IsNull())
    {
        if (!m_bEdit)
        {
            CRect rect(0, 0, m_image.GetWidth(), m_image.GetHeight());
            rect.OffsetRect(m_ptOffset);
            if (rect.PtInRect(point))
            {
                m_bDragging = TRUE;
                SetCapture();
            }
        }
        else
        {
            // 记录起始点
            startPoint = point;
            //isDrawing = true;
        }
        
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void CMyWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (!m_bEdit)
    {
        if (m_bDragging)
        {
            ReleaseCapture();
            m_bDragging = FALSE;
        }
    }
    else
    {
        // 如果正在绘制椭圆，结束绘制
        //if (isDrawing)
        //{
        //    // 绘制椭圆到图像上

        //    // 重置标志
        //    isDrawing = false;

        //    // 重绘窗口
        //    Invalidate();
        //}
    }
    

    CWnd::OnLButtonUp(nFlags, point);
}

void CMyWnd::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_bEdit)
    {
        if (m_bDragging && m_bDbClick)
        {
            CPoint ptDelta = point - m_ptLastMousePos;
            m_ptOffset += ptDelta;

            RECT rect;
            GetClientRect(&rect);
            if (m_ptOffset.x > 0)
            {
                m_ptOffset.x = 0;
            }
            if (m_ptOffset.y > 0)
            {
                m_ptOffset.y = 0;
            }
            if (m_ptOffset.x + m_image.GetWidth() < rect.right)
            {
                m_ptOffset.x = rect.right - m_image.GetWidth();
            }
            if (m_ptOffset.y + m_image.GetHeight() < rect.bottom)
            {
                m_ptOffset.y = rect.bottom - m_image.GetHeight();
            }

            Invalidate();
        }

        m_ptLastMousePos = point;
    }
    else
    {
        // 如果鼠标左键按下，并且正在移动，更新结束点并重绘
        if ((nFlags & MK_LBUTTON) && (startPoint != point))
        {
            // 更新结束点
            endPoint = point;

            // 重绘窗口
            Invalidate();
            // 重绘窗口
            //RedrawWindow();
        }

        /* 如果正在绘制椭圆，更新结束点并重绘
        if (isDrawing)
        {
             记录结束点
            endPoint = point;

             重绘窗口
            Invalidate();
        }*/
    }
    

    CWnd::OnMouseMove(nFlags, point);
}


void CMyWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (!m_bEdit)
    {
        m_bDbClick = !m_bDbClick;
        Invalidate();
    }
   
    CWnd::OnLButtonDblClk(nFlags, point);
}


void CMyWnd::SetEdit()
{
    m_bEdit = !m_bEdit;
}