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

    // ����ƽ������ģʽ
    dc.SetStretchBltMode(HALFTONE);

    // ����ͼ���ڵ�ǰ�����µ���ʾ�ߴ�
    int scaledWidth = static_cast<int>(m_image.GetWidth() * m_scaleFactor);
    int scaledHeight = static_cast<int>(m_image.GetHeight() * m_scaleFactor);

    // ����ͼƬ��ԭ����ȷ�����ᳬ���߽�
    AdjustImageOrigin();

    // ����ͼ�����ʾ����
    CRect destRect;
    destRect.left = m_imageOrigin.x;
    destRect.top = m_imageOrigin.y;
    destRect.right = destRect.left + scaledWidth;
    destRect.bottom = destRect.top + scaledHeight;

    // ����ͼ��
    m_image.Draw(dc, destRect);
}

BOOL CMyWnd2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // �����λ��ת��Ϊ��������
    ScreenToClient(&pt);

    // �������λ�������ͼƬ���Ͻǵ�λ��
    CPoint imgPos((pt.x - m_imageOrigin.x) / m_scaleFactor,
        (pt.y - m_imageOrigin.y) / m_scaleFactor);

    // ���ݹ��ֵķ��������������
    if (zDelta > 0)
        m_scaleFactor *= 1.1f; // �Ŵ�
    else
        m_scaleFactor /= 1.1f; // ��С

    // ������������
    LimitScalingFactor();

    // ����ͼƬ���Ͻǵ����꣬ʹ���ָ���λ�ù̶�����
    m_imageOrigin.x = pt.x - imgPos.x * m_scaleFactor;
    m_imageOrigin.y = pt.y - imgPos.y * m_scaleFactor;

    // ����ͼƬ��ԭ����ȷ�����ᳬ���߽�
    AdjustImageOrigin();

    // ���»��ƴ���
    Invalidate();
    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CMyWnd2::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_dragging = true;
    m_lastMousePos = point;
    SetCapture(); // ������꣬�����϶��¼�
    CWnd::OnLButtonDown(nFlags, point);
}

void CMyWnd2::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_dragging = false;
    ReleaseCapture(); // �ͷ���겶��
    CWnd::OnLButtonUp(nFlags, point);
}

void CMyWnd2::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_dragging)
    {
        // ��������ƶ��ľ���
        CPoint delta = point - m_lastMousePos;
        m_lastMousePos = point;

        // ����ͼƬ��ʾ�����ԭ��λ��
        m_imageOrigin += delta;

        // ����ͼƬ��ԭ����ȷ�����ᳬ���߽�
        AdjustImageOrigin();

        // ���»��ƴ���
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

    // ����ͼƬ�����Ͻ����꣬ʹͼƬ��Ե�����뿪���ڱ�Ե
    if (m_imageOrigin.x > 0)
        m_imageOrigin.x = 0;
    if (m_imageOrigin.y > 0)
        m_imageOrigin.y = 0;

    if (m_imageOrigin.x + scaledWidth < clientRect.Width())
        m_imageOrigin.x = clientRect.Width() - scaledWidth;

    if (m_imageOrigin.y + scaledHeight < clientRect.Height())
        m_imageOrigin.y = clientRect.Height() - scaledHeight;

    // ��ֹ���ڳߴ����ͼ��ߴ�������ͼƬ���Ĳ��ڴ�������
    if (scaledWidth < clientRect.Width())
        m_imageOrigin.x = (clientRect.Width() - scaledWidth) / 2;

    if (scaledHeight < clientRect.Height())
        m_imageOrigin.y = (clientRect.Height() - scaledHeight) / 2;
}

void CMyWnd2::LimitScalingFactor()
{
    CRect clientRect;
    GetClientRect(&clientRect);

    // �����������ӣ���ֹͼ����С���ȴ��ڻ�С
    float minScaleX = static_cast<float>(clientRect.Width()) / m_image.GetWidth();
    float minScaleY = static_cast<float>(clientRect.Height()) / m_image.GetHeight();
    float minScale = max(minScaleX, minScaleY);

    m_scaleFactor = max(m_scaleFactor, minScale);
    m_scaleFactor = min(m_scaleFactor, 10.0f); // Ҳ��������������ű���
}