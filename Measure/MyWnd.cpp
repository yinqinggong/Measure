#include "pch.h"
#include "MyWnd.h"
#include <iostream>

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
	// ����ͼƬ����"path_to_your_image"�滻Ϊ���ͼƬ·����
	m_image.Load(_T("..\\Doc\\test.jpg")); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��
	 //m_image.Load(_T("C:\\Users\\yinqi\\Desktop\\13.jpg")); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��
	m_ellipseRects.push_back(CRect(50, 50, 150, 100));
	m_ellipseRects.push_back(CRect(200, 50, 300, 100));
	m_ellipseRects.push_back(CRect(100, 150, 200, 200));
	m_ellipseRects.push_back(CRect(250, 150, 350, 200));
	m_ellipseRects.push_back(CRect(150, 250, 250, 300));
}

CMyWnd::~CMyWnd()
{
    
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
        if (!m_bEdit || (m_bEdit && m_isDrawFinished))
        {
            // ��Ҫ�Ȼ���ͼ�Σ��ٻ��Ʊ���ͼ��������ȷ��ʾ
            // ��ȡ��ͼ��������豸������
            CDC* pDC = CDC::FromHandle(m_image.GetDC());
            if (pDC)
            {
                // ���ƶ����������Բ����
                for (int i = 0; i < m_ellipseRects.size(); ++i)
                {
                    if (m_ellipseRects[i].TopLeft() == m_ellipseRects[i].BottomRight())
                    {
                        continue;
                    }
                    // ��������&ѡ�񻭱�
                    CPen pen(PS_SOLID, 2, RGB(255, 0, 0)); // 2Ϊ�߿�Ŀ�ȣ����Ը�����Ҫ����
                    CPen* pOldPen = pDC->SelectObject(&pen);

                    // ����һ���ջ�ˢ&ѡ��ջ�ˢ
                    CBrush brush;
                    brush.CreateStockObject(NULL_BRUSH);
                    CBrush* pOldBrush = pDC->SelectObject(&brush);

                    // ���Ʋ�������Բ����
                    pDC->Ellipse(m_ellipseRects[i]);

                    // ��ȡ��Բ�����ĵ�ͳ���������˵�
                    CPoint center(m_ellipseRects[i].CenterPoint());
                    CPoint end1(center.x + (m_ellipseRects[i].Width() / 2), center.y);
                    CPoint end2(center.x - (m_ellipseRects[i].Width() / 2), center.y);

                    // ���㳤��ĳ���
                    double distance = sqrt(pow(end1.x - end2.x, 2) + pow(end1.y - end2.y, 2));

                    // ������ĳ��Ȼ�������Բ���ĵ㸽��
                    CString strDistance;
                    strDistance.Format(_T("%.2f"), distance);
                    // �����ı�����ģʽΪ͸��
                    pDC->SetBkMode(TRANSPARENT);
                    pDC->TextOut(center.x - 20, center.y - 10, strDistance);

                    // �ָ�ԭ���Ļ��ʺͻ�ˢ
                    pDC->SelectObject(pOldPen);
                    pDC->SelectObject(pOldBrush);
                }
                // �ͷ��豸������
                m_image.ReleaseDC();
            }
            //��Բ���Ƴɹ����ٻ�����ͼƬ
            if (m_bDbClick)
            {
                m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
            }
            else
            {
                m_image.Draw(dc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_image.GetWidth(), m_image.GetHeight());
            }
        }
        else 
        {
            m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
            // ��������&ѡ�񻭱�
            CPen pen(PS_SOLID, 2, RGB(255, 255, 255)); // 2Ϊ�߿�Ŀ�ȣ����Ը�����Ҫ����
            CPen* pOldPen = dc.SelectObject(&pen);

            // ����һ���ջ�ˢ&ѡ��ջ�ˢ
            CBrush brush;
            brush.CreateStockObject(NULL_BRUSH);
            CBrush* pOldBrush = dc.SelectObject(&brush);

            // ������Բ��λ�úʹ�С&������Բ
            CRect rectEllipse(startPoint, endPoint);
            dc.Ellipse(rectEllipse);

            // �ָ�ԭ���Ļ��ʺͻ�ˢ
            dc.SelectObject(pOldPen);
            dc.SelectObject(pOldBrush);
        }
    }
    else
    {
        MessageBox(_T("��ͼʧ�ܣ�"));
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
            // ��¼��ʼ��
            m_isDrawFinished = false;
            startPoint = point;
            m_ellipseRects.push_back(CRect(startPoint, startPoint));
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
        m_isDrawFinished = true;
        Invalidate();
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
        // ������������£����������ƶ������½����㲢�ػ�
        if ((nFlags & MK_LBUTTON) && (startPoint != point))
        {
            // ���½�����
            endPoint = point;
            if (m_ellipseRects.size() > 0)
            {
                //�ƶ�֮��Ҫ���¼�����ʼ��
                CPoint newStartPoint(startPoint.x - m_ptOffset.x, startPoint.y - m_ptOffset.y);
                CPoint newEndPoint(endPoint.x - m_ptOffset.x, endPoint.y - m_ptOffset.y);
                m_ellipseRects[m_ellipseRects.size() - 1].SetRect(newStartPoint, newEndPoint);
            }
            // �ػ洰��
            Invalidate();
        }
    }

    CWnd::OnMouseMove(nFlags, point);
}


void CMyWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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