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
	m_bDragging = false;
    m_bDbClick = false;
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
		if (/*m_bDragging && */m_bDbClick)
		{
			m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
		}
		else
		{
			//m_image.Draw(dc, 0, 0);
            m_image.Draw(dc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_image.GetWidth(), m_image.GetHeight());
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
        CRect rect(0, 0, m_image.GetWidth(), m_image.GetHeight());
        rect.OffsetRect(m_ptOffset);
        if (rect.PtInRect(point))
        {
            m_bDragging = TRUE;
            SetCapture();
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void CMyWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bDragging)
    {
        ReleaseCapture();
        m_bDragging = FALSE;
    }

    CWnd::OnLButtonUp(nFlags, point);
}

void CMyWnd::OnMouseMove(UINT nFlags, CPoint point)
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

    CWnd::OnMouseMove(nFlags, point);
}


void CMyWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    m_bDbClick = !m_bDbClick;
    Invalidate();
    CWnd::OnLButtonDblClk(nFlags, point);
}
