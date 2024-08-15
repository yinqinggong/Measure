#include "pch.h"
#include "MyWnd2.h"

BEGIN_MESSAGE_MAP(CMyWnd2, CWnd)
    ON_WM_PAINT()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

CMyWnd2::CMyWnd2() : m_scaleFactor(1.0f), m_imageOrigin(0, 0), m_dragging(false)
{
}

CMyWnd2::~CMyWnd2()
{
}

BOOL CMyWnd2::LoadImage(LPCTSTR lpszPath)
{
    HRESULT hr = m_image.Load(lpszPath);
    if (FAILED(hr))
    {
        AfxMessageBox(_T("Failed to load image"));
        return FALSE;
    }
    Invalidate();
    return TRUE;
}

void CMyWnd2::OnPaint()
{
    CPaintDC dc(this);
    CRect clientRect;
    GetClientRect(&clientRect);

    // 设置平滑缩放模式
    dc.SetStretchBltMode(HALFTONE);

    // 计算图像在当前缩放下的显示尺寸
    int scaledWidth = static_cast<int>(m_image.GetWidth() * m_scaleFactor);
    int scaledHeight = static_cast<int>(m_image.GetHeight() * m_scaleFactor);

    // 调整图片的原点以确保不会超出边界
    AdjustImageOrigin();

    // 计算图像的显示区域
    CRect destRect;
    destRect.left = m_imageOrigin.x;
    destRect.top = m_imageOrigin.y;
    destRect.right = destRect.left + scaledWidth;
    destRect.bottom = destRect.top + scaledHeight;

    // 绘制图像
    m_image.Draw(dc, destRect);
}

BOOL CMyWnd2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // 将鼠标位置转换为窗口坐标
    ScreenToClient(&pt);

    // 计算鼠标位置相对于图片左上角的位置
    CPoint imgPos((pt.x - m_imageOrigin.x) / m_scaleFactor,
        (pt.y - m_imageOrigin.y) / m_scaleFactor);

    // 根据滚轮的方向调整缩放因子
    if (zDelta > 0)
        m_scaleFactor *= 1.1f; // 放大
    else
        m_scaleFactor /= 1.1f; // 缩小

    // 限制缩放因子
    LimitScalingFactor();

    // 调整图片左上角的坐标，使鼠标指针的位置固定不变
    m_imageOrigin.x = pt.x - imgPos.x * m_scaleFactor;
    m_imageOrigin.y = pt.y - imgPos.y * m_scaleFactor;

    // 调整图片的原点以确保不会超出边界
    AdjustImageOrigin();

    // 重新绘制窗口
    Invalidate();
    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CMyWnd2::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_dragging = true;
    m_lastMousePos = point;
    SetCapture(); // 捕获鼠标，处理拖动事件
    CWnd::OnLButtonDown(nFlags, point);
}

void CMyWnd2::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_dragging = false;
    ReleaseCapture(); // 释放鼠标捕获
    CWnd::OnLButtonUp(nFlags, point);
}

void CMyWnd2::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_dragging)
    {
        // 计算鼠标移动的距离
        CPoint delta = point - m_lastMousePos;
        m_lastMousePos = point;

        // 更新图片显示区域的原点位置
        m_imageOrigin += delta;

        // 调整图片的原点以确保不会超出边界
        AdjustImageOrigin();

        // 重新绘制窗口
        Invalidate();
    }
    CWnd::OnMouseMove(nFlags, point);
}

void CMyWnd2::AdjustImageOrigin()
{
    CRect clientRect;
    GetClientRect(&clientRect);

    int scaledWidth = static_cast<int>(m_image.GetWidth() * m_scaleFactor);
    int scaledHeight = static_cast<int>(m_image.GetHeight() * m_scaleFactor);

    // 限制图片的左上角坐标，使图片边缘不会离开窗口边缘
    if (m_imageOrigin.x > 0)
        m_imageOrigin.x = 0;
    if (m_imageOrigin.y > 0)
        m_imageOrigin.y = 0;

    if (m_imageOrigin.x + scaledWidth < clientRect.Width())
        m_imageOrigin.x = clientRect.Width() - scaledWidth;

    if (m_imageOrigin.y + scaledHeight < clientRect.Height())
        m_imageOrigin.y = clientRect.Height() - scaledHeight;

    // 防止窗口尺寸大于图像尺寸的情况下图片中心不在窗口中心
    if (scaledWidth < clientRect.Width())
        m_imageOrigin.x = (clientRect.Width() - scaledWidth) / 2;

    if (scaledHeight < clientRect.Height())
        m_imageOrigin.y = (clientRect.Height() - scaledHeight) / 2;
}

void CMyWnd2::LimitScalingFactor()
{
    CRect clientRect;
    GetClientRect(&clientRect);

    // 限制缩放因子，防止图像缩小到比窗口还小
    float minScaleX = static_cast<float>(clientRect.Width()) / m_image.GetWidth();
    float minScaleY = static_cast<float>(clientRect.Height()) / m_image.GetHeight();
    float minScale = max(minScaleX, minScaleY);

    m_scaleFactor = max(m_scaleFactor, minScale);
    m_scaleFactor = min(m_scaleFactor, 10.0f); // 也可以限制最大缩放比例
}