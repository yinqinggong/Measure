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
	// ����ͼƬ����"path_to_your_image"�滻Ϊ���ͼƬ·����
	m_image.Load(_T("..\\Doc\\test.jpg")); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��
	 //m_image.Load(_T("C:\\Users\\yinqi\\Desktop\\13.jpg")); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��
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
            // ��ȡ��ͼ��������豸������
            CDC* pDC = CDC::FromHandle(m_image.GetDC());
            if (pDC)
            {
                // ���������ɫ�Ļ���
                COLORREF colors[5] = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 0), RGB(255, 0, 255) };

                // ���������Բ��λ�úʹ�С
                CRect rectEllipses[5] = {
                    CRect(50, 50, 150, 100),
                    CRect(200, 50, 300, 100),
                    CRect(100, 150, 200, 200),
                    CRect(250, 150, 350, 200),
                    CRect(150, 250, 250, 300)
                };

                // ���ƶ����������Բ����
                for (int i = 0; i < 5; ++i)
                {
                    // ��������
                    CPen pen(PS_SOLID, 2, colors[i]); // 2Ϊ�߿�Ŀ�ȣ����Ը�����Ҫ����

                    // ѡ�񻭱�
                    CPen* pOldPen = pDC->SelectObject(&pen);

                    // ����һ���ջ�ˢ
                    CBrush brush;
                    brush.CreateStockObject(NULL_BRUSH);

                    // ѡ��ջ�ˢ
                    CBrush* pOldBrush = pDC->SelectObject(&brush);

                    // ���Ʋ�������Բ����
                    pDC->Ellipse(rectEllipses[i]);

                    // ��ȡ��Բ�����ĵ�ͳ���������˵�
                    CPoint center(rectEllipses[i].CenterPoint());
                    CPoint end1(center.x + (rectEllipses[i].Width() / 2), center.y);
                    CPoint end2(center.x - (rectEllipses[i].Width() / 2), center.y);

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
        }
        else 
        {
            m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
            // ��������
            CPen pen(PS_SOLID, 2, RGB(255, 255, 255)); // 2Ϊ�߿�Ŀ�ȣ����Ը�����Ҫ����

            // ѡ�񻭱�
            CPen* pOldPen = dc.SelectObject(&pen);

            // ����һ���ջ�ˢ
            CBrush brush;
            brush.CreateStockObject(NULL_BRUSH);

            // ѡ��ջ�ˢ
            CBrush* pOldBrush = dc.SelectObject(&brush);

            // ������Բ��λ�úʹ�С
            CRect rectEllipse(startPoint, endPoint);
            // ����֮ǰ���Ƶ���Բ
            //dc.FillRect(CRect(0, 0, 10000, 10000), CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));

            // ������Բ
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
        // ������ڻ�����Բ����������
        //if (isDrawing)
        //{
        //    // ������Բ��ͼ����

        //    // ���ñ�־
        //    isDrawing = false;

        //    // �ػ洰��
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
        // ������������£����������ƶ������½����㲢�ػ�
        if ((nFlags & MK_LBUTTON) && (startPoint != point))
        {
            // ���½�����
            endPoint = point;

            // �ػ洰��
            Invalidate();
            // �ػ洰��
            //RedrawWindow();
        }

        /* ������ڻ�����Բ�����½����㲢�ػ�
        if (isDrawing)
        {
             ��¼������
            endPoint = point;

             �ػ洰��
            Invalidate();
        }*/
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