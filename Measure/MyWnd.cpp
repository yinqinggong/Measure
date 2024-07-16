#include "pch.h"
#include "MyWnd.h"
#include <iostream>
#include "CDlgDiameter.h"
#include "base64.h"
#include "LogFile.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <libavutil/mathematics.h>
#include "common.h"

#define IDC_BTN_CAPTURE                 8000+1
#define IDC_BTN_REC                     8000+2
#define IDC_BTN_DIS                     8000+3

BEGIN_MESSAGE_MAP(CMyWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_WM_LBUTTONDBLCLK()
    ON_BN_CLICKED(IDC_BTN_CAPTURE, &CMyWnd::OnBnClickedBtnCapture)
    ON_BN_CLICKED(IDC_BTN_REC, &CMyWnd::OnBnClickedBtnRec)
    ON_BN_CLICKED(IDC_BTN_DIS, &CMyWnd::OnBnClickedBtnDis)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CMyWnd::CMyWnd()
{
	m_bDragging = FALSE;
    m_bDbClick = FALSE;
    m_status = 0;
    m_isPolygonComplete = false;
    m_scaleWood = { 0 };
    m_ellipse_add = { 0 };
}

CMyWnd::~CMyWnd()
{
    
}
int CMyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    m_btnCapture.Create(_T("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_CAPTURE);
    m_btnRec.Create(_T("ʶ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_REC);
    m_btnDis.Create(_T("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_DIS);

    CRect rect;
    GetClientRect(&rect);
    m_btnCapture.MoveWindow(rect.right - 120, rect.Height() * 0.5, 100, 50);
    m_btnRec.MoveWindow(rect.right - 120, rect.Height() * 0.5 - 100, 100, 50);
    m_btnDis.MoveWindow(rect.right - 120, rect.Height() * 0.5 + 100, 100, 50);
    return 0;
}

void CMyWnd::DrawRotatedEllipse(Gdiplus::Graphics* graphics, WoodAttr &woodAttr)
{
    // ���������ñ任����
    Gdiplus::Matrix matrix;
    matrix.Translate(woodAttr.ellipse.cx, woodAttr.ellipse.cy);
    matrix.Rotate(woodAttr.ellipse.angel);

    // ����ͼ��״̬��Ӧ�ñ任
    GraphicsState state = graphics->Save();
    graphics->SetTransform(&matrix);

    // ������Բ
    if (woodAttr.isDeleting)
    {
        Pen red_pen(Gdiplus::Color(255, 255, 0, 0), 4);
        graphics->DrawEllipse(&red_pen, (int)-woodAttr.ellipse.ab1, (int)-woodAttr.ellipse.ab2, 2 * (int)woodAttr.ellipse.ab1, 2 * (int)woodAttr.ellipse.ab2);
    }
    else
    {
        Pen green_pen(Gdiplus::Color(255, 0, 255, 0), 4);
        graphics->DrawEllipse(&green_pen, (int)-woodAttr.ellipse.ab1, (int)-woodAttr.ellipse.ab2, 2 * (int)woodAttr.ellipse.ab1, 2 * (int)woodAttr.ellipse.ab2);
    }

    // �ָ�ͼ��״̬
    graphics->Restore(state);

    // �������֣��̰��᳤�ȣ�
    CString text;
    text.Format(_T("%.2f"), woodAttr.diameter);
    Gdiplus::FontFamily fontFamily(L"Arial");
    Gdiplus::Font font(&fontFamily, 12, FontStyleRegular, UnitPixel);
    Gdiplus::PointF pointF(static_cast<float>(woodAttr.ellipse.cx), static_cast<float>(woodAttr.ellipse.cy));
    Gdiplus::SolidBrush solidBrush(Gdiplus::Color(255, 0, 0, 255)); // ��ɫ��ˢ
    graphics->DrawString(text, -1, &font, pointF, &solidBrush);

    // �ָ�ͼ��״̬����ȷ����������ͼ�β���Ӱ��
   // Matrix matrix0;
    //graphics->SetTransform(&matrix0); // ���ñ任����
}

void CMyWnd::DrawRotatedEllipse(Gdiplus::Graphics* graphics, Gdiplus::Pen& pen, int cx, int cy, int ab1, int ab2, double angle)
{
   // ���������ñ任����
    Gdiplus::Matrix matrix;
    matrix.Translate(cx, cy);
    matrix.Rotate(angle);

    // ����ͼ��״̬��Ӧ�ñ任
    GraphicsState state = graphics->Save();
    graphics->SetTransform(&matrix);

    // ������Բ
    graphics->DrawEllipse(&pen, -ab1, -ab2, 2 * ab1, 2 * ab2);

    // �ָ�ͼ��״̬
    graphics->Restore(state);
}

void CMyWnd::DrawPolygon(Gdiplus::Graphics* graphics, PointF points[], int numPoints)
{
    Pen red_pen(Gdiplus::Color(255, 255, 0, 0), 2);
    graphics->DrawPolygon(&red_pen, points, numPoints);
}

void CMyWnd::OnPaint()
{
	CPaintDC dc(this);

	if (!m_image.IsNull())
	{
        CRect rect;
        GetClientRect(&rect);
        if (m_status == 0 || (m_status == 1 && m_isDrawFinished) || m_status == 2)
        {
            // ��ȡGDI+ͼ�ζ���
            Gdiplus::Graphics graphics(m_image.GetDC());

            for (size_t i = 0; i < m_scaleWood.wood_list.size(); i++)
            {
                DrawRotatedEllipse(&graphics, m_scaleWood.wood_list[i]);
            }

            // ����������
            if (m_points.size() > 0)
            {
                Gdiplus::PointF* points = new Gdiplus::PointF[m_points.size()];
                for (size_t i = 0; i < m_points.size(); i++)
                {
                    points[i].X = m_points[i].x;
                    points[i].Y = m_points[i].y;
                }
                DrawPolygon(&graphics, points, m_points.size());
                delete[]points;
            }

            // �ͷ�GDI+ͼ�ζ���
            m_image.ReleaseDC();

            // �����Ƶ�ͼ����ʾ��������
            //dc.BitBlt(0, 0, m_image.GetWidth(), m_image.GetHeight(), CDC::FromHandle(m_image.GetDC()), 0, 0, SRCCOPY);

            //// ��Ҫ�Ȼ���ͼ�Σ��ٻ��Ʊ���ͼ��������ȷ��ʾ
            //// ��Բ���Ƴɹ����ٻ�����ͼƬ
            //// ��ȡ��ͼ��������豸������
            //CDC* pDC = CDC::FromHandle(m_image.GetDC());
            //if (pDC)
            //{
            //    // ���ƶ����������Բ����
            //    for (int i = 0; i < m_ellipseRects.size(); ++i)
            //    {
            //        if (m_ellipseRects[i].rect.TopLeft() == m_ellipseRects[i].rect.BottomRight())
            //        {
            //            continue;
            //        }
            //        // ��������&ѡ�񻭱�
            //        CPen pen_red(PS_SOLID, 5, RGB(255, 0, 0)); // 2Ϊ�߿�Ŀ�ȣ����Ը�����Ҫ���� 
            //        CPen pen_green(PS_SOLID, 5, RGB(0, 255, 0)); // 2Ϊ�߿�Ŀ�ȣ����Ը�����Ҫ����
            //        CPen* pOldPen;
            //        if (m_ellipseRects[i].isDeleting)
            //        {
            //            pOldPen = pDC->SelectObject(&pen_red);
            //        }
            //        else
            //        {
            //            pOldPen = pDC->SelectObject(&pen_green);
            //        }
            //    
            //        // ����һ���ջ�ˢ&ѡ��ջ�ˢ
            //        CBrush brush;
            //        brush.CreateStockObject(NULL_BRUSH);
            //        CBrush* pOldBrush = pDC->SelectObject(&brush);

            //        // ���Ʋ�������Բ����
            //        pDC->Ellipse(m_ellipseRects[i].rect);

            //        // ��ȡ��Բ�����ĵ�ͳ���������˵�
            //        CPoint center(m_ellipseRects[i].rect.CenterPoint());
            //        //CPoint end1(center.x + (m_ellipseRects[i].rect.Width() / 2), center.y);
            //        //CPoint end2(center.x - (m_ellipseRects[i].rect.Width() / 2), center.y);
            //        //// ���㳤��ĳ���
            //        //double distance = sqrt(pow(end1.x - end2.x, 2) + pow(end1.y - end2.y, 2));
            //        if (m_ellipseRects[i].diameter > 0.0001)
            //        {
            //            // ������ĳ��Ȼ�������Բ���ĵ㸽��
            //            CString strDistance;
            //            strDistance.Format(_T("%.2f"), m_ellipseRects[i].diameter);
            //            // �����ı�����ģʽΪ͸��
            //            pDC->SetBkMode(TRANSPARENT);
            //            pDC->TextOut(center.x - 20, center.y - 10, strDistance);
            //        }

            //        // �ָ�ԭ���Ļ��ʺͻ�ˢ
            //        pDC->SelectObject(pOldPen);
            //        pDC->SelectObject(pOldBrush);
            //    }

            //    // ����͸������ģʽ
            //    pDC->SetBkMode(TRANSPARENT);
            //    // �������ʺͻ�ˢ
            //    CPen pen(PS_SOLID, 5, RGB(255, 0, 0)); // ��ɫ�߿�
            //    CBrush* pOldBrush = static_cast<CBrush*>(pDC->SelectStockObject(NULL_BRUSH)); // �ջ�ˢ
            //    CPen* pOldPen = pDC->SelectObject(&pen);

            //    // ���ƶ����
            //    if (m_points.size() > 1)
            //    {
            //        for (size_t i = 0; i < m_points.size() - 1; i++)
            //        {
            //            pDC->MoveTo(m_points[i]);
            //            pDC->LineTo(m_points[i + 1]);
            //        }
            //        //dc.Polyline(&m_points[0], static_cast<int>(m_points.size()));
            //        if (m_isPolygonComplete)
            //        {
            //            pDC->MoveTo(m_points.front());
            //            pDC->LineTo(m_points.back());
            //        }
            //    }
            //    // �ָ�ԭ���Ļ��ʺͻ�ˢ
            //    pDC->SelectObject(pOldPen);
            //    pDC->SelectObject(pOldBrush);


            //    // �ͷ��豸������
            //    m_image.ReleaseDC();
            //}
            //��Բ���Ƴɹ����ٻ�����ͼƬ
            ::SetStretchBltMode(dc, HALFTONE);
            ::SetBrushOrgEx(dc, 0, 0, NULL);
            if (m_bDbClick)
            {
                m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
            }
            else
            {
                m_image.Draw(dc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_image.GetWidth(), m_image.GetHeight());
            }
        }
        else if(m_status == 1)
        {
            ::SetStretchBltMode(dc, HALFTONE);
            ::SetBrushOrgEx(dc, 0, 0, NULL);
            if (m_bDbClick)
            {
                m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
            }
            else
            {
                m_image.Draw(dc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_image.GetWidth(), m_image.GetHeight());
            }

            // ʹ��˫�����ͼ����ֹ��˸
            /*CRect rect;
            GetClientRect(&rect);
            CDC memDC;
            CBitmap bmp;
            memDC.CreateCompatibleDC(&dc);
            bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
            CBitmap* pOldbmp = memDC.SelectObject(&bmp);*/

            // ��ȡGDI+ͼ�ζ���
            Gdiplus::Graphics graphics(dc.GetSafeHdc());

            // ����һ������
            Pen pen(Gdiplus::Color(255, 255, 255, 255), 2);
            int cx = (m_startPoint.x + m_endPoint.x) * 0.5;
            int cy = (m_startPoint.y + m_endPoint.y) * 0.5;
            int ab1 = abs(m_endPoint.x - m_startPoint.x) * 0.5;
            int ab2 = abs(m_endPoint.y - m_startPoint.y) * 0.5;
            double angel = CalculateAngle(m_startPoint.x, m_startPoint.y, m_endPoint.x, m_endPoint.y);
            DrawRotatedEllipse(&graphics, pen, cx, cy, ab1, ab2, angel);


            // ������ڻ�����Բ�������֮
            //if (m_isDrawing)
            //{
            //    // ������Բ����������ͳ��̰��᳤��
            //    int cx = (startPoint.x + endPoint.x) / 2;
            //    int cy = (startPoint.y + endPoint.y) / 2;
            //    int ab1 = abs(endPoint.x - startPoint.x) / 2;
            //    int ab2 = abs(endPoint.y - startPoint.y) / 2;

            //    // ������Բ
            //    DrawRotatedEllipse2(&graphics, pen, cx, cy, ab1, ab2, m_angle);
            //}

            // �����Ƶ�ͼ�񿽱������ڵ�DC��
            //dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

            // ����
            //memDC.SelectObject(pOldbmp);


            //// ��������&ѡ�񻭱�
            //CPen pen(PS_SOLID, 2, RGB(255, 255, 255)); // 2Ϊ�߿�Ŀ�ȣ����Ը�����Ҫ����
            //CPen* pOldPen = dc.SelectObject(&pen);

            //// ����һ���ջ�ˢ&ѡ��ջ�ˢ
            //CBrush brush;
            //brush.CreateStockObject(NULL_BRUSH);
            //CBrush* pOldBrush = dc.SelectObject(&brush);

            //// ������Բ��λ�úʹ�С&������Բ
            //CRect rectEllipse(startPoint, endPoint);
            //dc.Ellipse(rectEllipse);

            //// �ָ�ԭ���Ļ��ʺͻ�ˢ
            //dc.SelectObject(pOldPen);
            //dc.SelectObject(pOldBrush);
        }
        else if (m_status == 3)
        {
            ::SetStretchBltMode(dc, HALFTONE);
            ::SetBrushOrgEx(dc, 0, 0, NULL);
            if (m_bDbClick)
            {
                m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
            }
            else
            {
                m_image.Draw(dc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_image.GetWidth(), m_image.GetHeight());
            }
            // ����͸������ģʽ
            dc.SetBkMode(TRANSPARENT);
            // �������ʺͻ�ˢ
            CPen pen(PS_SOLID, 2, RGB(255, 255, 255)); // ��ɫ�߿�
            CBrush* pOldBrush = static_cast<CBrush*>(dc.SelectStockObject(NULL_BRUSH)); // �ջ�ˢ
            CPen* pOldPen = dc.SelectObject(&pen);

            // ���ƶ����
            if (m_points.size() > 1)
            {
                for (size_t i = 0; i < m_points.size() - 1; i++)
                {
                    dc.MoveTo(m_points[i]);
                    dc.LineTo(m_points[i + 1]);
                }
                //dc.Polyline(&m_points[0], static_cast<int>(m_points.size()));
                if (m_isPolygonComplete)
                {
                    dc.MoveTo(m_points.front());
                    dc.LineTo(m_points.back());
                }
            }
            // �ָ�ԭ���Ļ��ʺͻ�ˢ
            dc.SelectObject(pOldPen);
            dc.SelectObject(pOldBrush);
        }
    }
	else
	{
	    CRect rect;
	    GetClientRect(rect);
	    dc.FillSolidRect(rect, RGB(42, 42, 43));   //�ؼ�����ɫ
	    m_btnCapture.ShowWindow(SW_SHOW);
	    m_btnDis.ShowWindow(SW_HIDE);
	    m_btnRec.ShowWindow(SW_HIDE);
	}
}

void CMyWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_image.IsNull())
    {
        if (m_status == 0)
        {
            CRect rect(0, 0, m_image.GetWidth(), m_image.GetHeight());
            rect.OffsetRect(m_ptOffset);
            if (rect.PtInRect(point))
            {
                m_bDragging = TRUE;
                SetCapture();
            }
        }
        else if (m_status == 1)
        {
            // ��¼��ʼ��
            m_isDrawFinished = false;
            m_startPoint = point;
           /* m_ellipse_add.cx = startPoint.x;
            m_ellipse_add.cy = startPoint.y;
            m_ellipse_add.ab1 = 0;
            m_ellipse_add.ab2 = 0;
            m_ellipse_add.angel = 0;*/

            //m_ellipseRects.push_back({ CRect(startPoint, startPoint),0.0 });
        }
        else if (m_status == 2)
        {
            //�жϵ�ǰ���Ƿ���ĳ����Բ�ڲ�
            if (m_bDbClick)
            {
                CPoint curPoint(point.x - m_ptOffset.x, point.y - m_ptOffset.y);
                if (isPointInEllipse(curPoint.x, curPoint.y))
                {
                    SetFocus();
                    Invalidate();
                }
            }
            else
            {
                RECT rect;
                GetClientRect(&rect);   
                CPoint curPoint(point.x * m_image.GetWidth() * 1.0 / (rect.right - rect.left), 
                    point.y * m_image.GetHeight() * 1.0 / (rect.bottom - rect.top));
                if (isPointInEllipse(curPoint.x, curPoint.y))
                {
                    SetFocus();
                    Invalidate();
                }
            }
        }
        else if (m_status == 3)
        {
            if (!m_isPolygonComplete)
            {
                // ������ʱ����¼����
                if (m_points.size() > 0 && isCloseEnough(m_points.front(), point, 10))
                {
                    // ������һ�������һ�������С��5������ɶ����
                    m_points[m_points.size() - 1].SetPoint(m_points.front().x, m_points.front().y);
                    int ret = AfxMessageBox(_T("ȷ�ϱ�������������ľ�ģ�"), MB_OKCANCEL);
                    if (ret != IDOK)
                    {
                        m_points.clear();
                    }
                    else
                    {
                        m_isPolygonComplete = true;
                        if (m_bDbClick)
                        {
                            for (size_t i = 0; i < m_points.size(); i++)
                            {
                                m_points[i].x = m_points[i].x - m_ptOffset.x;
                                m_points[i].y = m_points[i].y - m_ptOffset.y;
                            }
                        }
                        else
                        {
                            RECT rect;
                            GetClientRect(&rect);
                            for (size_t i = 0; i < m_points.size(); i++)
                            {
                                m_points[i].x = m_points[i].x * m_image.GetWidth() * 1.0 / (rect.right - rect.left);
                                m_points[i].y = m_points[i].y * m_image.GetHeight() * 1.0 / (rect.bottom - rect.top);
                            }
                        }
                        //���ڶ����֮�������ֱ���޳�
                        std::vector<WoodAttr> temp_wood_list;
                        for (size_t i = 0; i < m_scaleWood.wood_list.size(); i++)
                        {
                            if (IsPointInPolygon(m_scaleWood.wood_list[i].ellipse.cx, 
                                m_scaleWood.wood_list[i].ellipse.cy, m_points))
                            {
                                temp_wood_list.push_back(m_scaleWood.wood_list[i]);
                            }
                        }
                        m_scaleWood.wood_list = temp_wood_list;
                        //֮ǰ�Ļ��Ƶ���Բ��Ч�ˣ���Ҫ���»���
                        m_image.Destroy();
                        CString imagePath;
                        imagePath.Format(_T("%s%d.jpg"), GetImagePath(), m_scaleWood.id);
                        m_image.Load(imagePath); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��

                        SetStatus(0);
                    }
                }
                else
                {
                    m_points.push_back(point);
                }
                Invalidate();
            }
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void CMyWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_status == 0)
    {
        if (m_bDragging)
        {
            ReleaseCapture();
            m_bDragging = FALSE;
        }
    }
    else if(m_status == 1)
    {
        if (m_ellipse_add.ab1 > 0.0 || m_ellipse_add.ab2 > 0.0)
        {
            //��Ҫ�û��������ֱ��
            CDlgDiameter dlg;
            INT_PTR nResponse = dlg.DoModal();
            if (nResponse == IDOK)
            {
                float d = dlg.GetDiameter();
                if (d > 0.001)
                {
                    WoodAttr woodAttr = { 0 };
                    woodAttr.diameter = d;
                    woodAttr.ellipse = m_ellipse_add;
                    woodAttr.diameters.d1 = m_ellipse_add.ab1;
                    woodAttr.diameters.d2 = m_ellipse_add.ab2;
                    m_scaleWood.wood_list.push_back(woodAttr);
                }
            }
        }
		//if (m_ellipseRects[m_ellipseRects.size() - 1].rect.TopLeft() != m_ellipseRects[m_ellipseRects.size() - 1].rect.BottomRight())
		//{
		//	//��Ҫ�û��������ֱ��
		//	CDlgDiameter dlg;
		//	INT_PTR nResponse = dlg.DoModal();
		//	if (nResponse == IDOK)
		//	{
		//		float d = dlg.GetDiameter();
		//		if (d > 0.001)
		//		{
		//			m_ellipseRects[m_ellipseRects.size() - 1].diameter = d;
		//		}
		//		else
		//		{
		//			m_ellipseRects.erase(--m_ellipseRects.end());
		//		}
		//	}
		//	else
		//	{
		//		m_ellipseRects.erase(--m_ellipseRects.end());
		//	}
  //      }
  //      else
  //      {
  //          m_ellipseRects.erase(--m_ellipseRects.end());
  //      }
        SetStatus(0);
        m_isDrawFinished = true;
        Invalidate();
    }

    CWnd::OnLButtonUp(nFlags, point);
}

void CMyWnd::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_status == 0)
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
    else if(m_status == 1)
    {
        // ������������£����������ƶ������½����㲢�ػ�
        if ((nFlags & MK_LBUTTON) && (m_startPoint != point))
        {
            // ���½�����
            m_endPoint = point;
            //if (m_ellipse_add.cx > 0.0)
            {
				//�϶�֮��Ҫ���¼�����ʼ��
				CPoint newStartPoint;
				CPoint newEndPoint;
				if (m_bDbClick)
				{
					newStartPoint.x = m_startPoint.x - m_ptOffset.x;
					newStartPoint.y = m_startPoint.y - m_ptOffset.y;
					newEndPoint.x = m_endPoint.x - m_ptOffset.x;
					newEndPoint.y = m_endPoint.y - m_ptOffset.y;
				}
				else
				{
					RECT rect;
					GetClientRect(&rect);
					newStartPoint.x = m_startPoint.x * m_image.GetWidth() * 1.0 / (rect.right - rect.left);
					newStartPoint.y = m_startPoint.y * m_image.GetHeight() * 1.0 / (rect.bottom - rect.top);
					newEndPoint.x = m_endPoint.x * m_image.GetWidth() * 1.0 / (rect.right - rect.left);
					newEndPoint.y = m_endPoint.y * m_image.GetHeight() * 1.0 / (rect.bottom - rect.top);
				}
				m_ellipse_add.cx = (newStartPoint.x + newEndPoint.x) * 0.5;
				m_ellipse_add.cy = (newStartPoint.y + newEndPoint.y) * 0.5;
				m_ellipse_add.ab1 = abs(newEndPoint.x - newStartPoint.x) * 0.5;
				m_ellipse_add.ab2 = abs(newEndPoint.y - newStartPoint.y) * 0.5;
				m_ellipse_add.angel = CalculateAngle(newStartPoint.x, newStartPoint.y, newEndPoint.x, newEndPoint.y);
                //m_ellipse_add.angel = CalculateAngle(m_startPoint.x, m_startPoint.y, m_endPoint.x, m_endPoint.y);
			}
            //if (m_ellipseRects.size() > 0)
            //{  
            //    //�϶�֮��Ҫ���¼�����ʼ��
            //    if (m_bDbClick)
            //    {
            //        CPoint newStartPoint(startPoint.x - m_ptOffset.x, startPoint.y - m_ptOffset.y);
            //        CPoint newEndPoint(endPoint.x - m_ptOffset.x, endPoint.y - m_ptOffset.y);
            //        m_ellipseRects[m_ellipseRects.size() - 1].rect.SetRect(newStartPoint, newEndPoint);
            //        // ����Ƕ�
            //        m_angle = CalculateAngle(newStartPoint.x, newStartPoint.y, newEndPoint.x, newEndPoint.y);
            //    }
            //    else
            //    {
            //        RECT rect;
            //        GetClientRect(&rect);
            //        CPoint newStartPoint(startPoint.x * m_image.GetWidth() * 1.0 / (rect.right - rect.left), 
            //            startPoint.y * m_image.GetHeight() * 1.0 / (rect.bottom - rect.top));
            //        CPoint newEndPoint(endPoint.x * m_image.GetWidth() * 1.0 / (rect.right - rect.left),
            //            endPoint.y * m_image.GetHeight() * 1.0 / (rect.bottom - rect.top));
            //        m_ellipseRects[m_ellipseRects.size() - 1].rect.SetRect(newStartPoint, newEndPoint);
            //        // ����Ƕ�
            //        m_angle = CalculateAngle(newStartPoint.x, newStartPoint.y, newEndPoint.x, newEndPoint.y);
            //    }
            //}
            Invalidate();
        }
    }
    else if (m_status == 3)
    {
        //��갴�µ�һ���������ƶ��ȼ���ڶ�����
        if (!m_isPolygonComplete && m_points.size() == 1)
        {
            m_points.push_back(point);
        }
        //��������ƶ��������޸����һ����
        if (!m_isPolygonComplete && m_points.size() > 1)
        {
            m_points[m_points.size() - 1].SetPoint(point.x, point.y);
        }

        Invalidate();
    }

    CWnd::OnMouseMove(nFlags, point);
}


void CMyWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if (m_status == 0)
    {
        m_bDbClick = !m_bDbClick;
        Invalidate();
    }
   
    CWnd::OnLButtonDblClk(nFlags, point);
}

bool CMyWnd::isCloseEnough(const CPoint& p1, const CPoint& p2, int threshold)
{
    int dx = p1.x - p2.x;
    int dy = p1.y - p2.y;
    return (dx * dx + dy * dy) <= (threshold * threshold);
}

bool CMyWnd::isPointInEllipse(const CPoint& p)
{
    for (size_t i = 0; i < m_scaleWood.wood_list.size(); i++)
    {
        if (p.x >= m_scaleWood.wood_list[i].ellipse.cx - m_scaleWood.wood_list[i].ellipse.ab1 * 0.5 &&
            p.x <= m_scaleWood.wood_list[i].ellipse.cx + m_scaleWood.wood_list[i].ellipse.ab1 * 0.5 &&
            p.y >= m_scaleWood.wood_list[i].ellipse.cy - m_scaleWood.wood_list[i].ellipse.ab2 * 0.5 &&
            p.y <= m_scaleWood.wood_list[i].ellipse.cy + m_scaleWood.wood_list[i].ellipse.ab2 * 0.5)
        {
            m_scaleWood.wood_list[i].isDeleting = !m_scaleWood.wood_list[i].isDeleting;
            return true;
        }
    }
    return false;
}

bool CMyWnd::isPointInEllipse(int px, int py)
{
    for (size_t i = 0; i < m_scaleWood.wood_list.size(); i++)
    {
        double angle = m_scaleWood.wood_list[i].ellipse.angel;
        int cx = m_scaleWood.wood_list[i].ellipse.cx;
        int cy = m_scaleWood.wood_list[i].ellipse.cy;
        int a = m_scaleWood.wood_list[i].ellipse.ab1;
        int b = m_scaleWood.wood_list[i].ellipse.ab2;

        // ���Ƕ�ת��Ϊ����
        double radianAngle = angle * M_PI / 180.0;

        // ����(px, py)����Բ����(cx, cy)������תangle�Ƕ�
        double cosAngle = cos(-radianAngle);
        double sinAngle = sin(-radianAngle);

        double dx = px - cx;
        double dy = py - cy;

        double rotatedX = dx * cosAngle - dy * sinAngle;
        double rotatedY = dx * sinAngle + dy * cosAngle;

        // �ж���ת��ĵ��Ƿ��ڱ�׼��Բ��
        double normalizedX = rotatedX / a;
        double normalizedY = rotatedY / b;
        if ((normalizedX * normalizedX + normalizedY * normalizedY) <= 1.0)
        {
            m_scaleWood.wood_list[i].isDeleting = !m_scaleWood.wood_list[i].isDeleting;
            return true;
        }
    }
    return false;
}

bool CMyWnd::IsPointInEllipse(int px, int py, int cx, int cy, int a, int b, double angle)
{
    // ���Ƕ�ת��Ϊ����
    double radianAngle = angle * M_PI / 180.0;

    // ����(px, py)����Բ����(cx, cy)������תangle�Ƕ�
    double cosAngle = cos(-radianAngle);
    double sinAngle = sin(-radianAngle);

    double dx = px - cx;
    double dy = py - cy;

    double rotatedX = dx * cosAngle - dy * sinAngle;
    double rotatedY = dx * sinAngle + dy * cosAngle;

    // �ж���ת��ĵ��Ƿ��ڱ�׼��Բ��
    double normalizedX = rotatedX / a;
    double normalizedY = rotatedY / b;

    return (normalizedX * normalizedX + normalizedY * normalizedY) <= 1.0;
}

bool CMyWnd::IsPointInPolygon(int px, int py, const std::vector<CPoint>& polygon)
{
    int n = polygon.size();
    bool inside = false;

    for (int i = 0, j = n - 1; i < n; j = i++)
    {
        int xi = polygon[i].x, yi = polygon[i].y;
        int xj = polygon[j].x, yj = polygon[j].y;

        bool intersect = ((yi > py) != (yj > py)) &&
            (px < (xj - xi)* (py - yi) / (yj - yi) + xi);
        if (intersect)
        {
            inside = !inside;
        }
    }

    return inside;
}

double CMyWnd::CalculateAngle(int startX, int startY, int endX, int endY)
{
    // ��������֮��ĽǶȣ��Զ�Ϊ��λ��
    double dx = endX - startX;
    double dy = endY - startY;
    double angle = atan2(dy, dx) * 180.0 / M_PI;

    return angle;
}

void CMyWnd::OnBnClickedBtnCapture()
{
#if (QGDebug == 1)
    ShowWindow(SW_SHOW);
#else
    ShowWindow(SW_SHOW);
    std::string limg;
    CWaitCursor wait;
    int ret = PostPhoto(limg);
    wait.Restore();
    if (ret < 0)
    {
        AfxMessageBox(_T("����ʧ�ܣ�������"));
        return;
    }
    try
    {
        limg = base64_decode(limg);
    }
    catch (const std::exception&)
    {
        WriteLog(_T("invalid base64"));
        AfxMessageBox(_T("��ȡͼƬʧ�ܣ�������"));
        return;
    }
    if (limg.length() <= 0)
    {
        AfxMessageBox(_T("��ȡͼƬʧ�ܣ�������"));
        return;
    }

	std::vector<uchar> img_data(limg.begin(), limg.end());
	cv::Mat img = cv::imdecode(cv::Mat(img_data), cv::IMREAD_COLOR);
	//cv::imwrite("..\\Doc\\limg.jpg", img);
    cv::imwrite(GetImagePathUTF8() + ("limg.jpg"));
#endif // 
    //m_image.Load(_T("..\\Doc\\limg.jpg")); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��
    m_image.Load(GetImagePath() + _T("limg.jpg"));
    m_btnCapture.ShowWindow(SW_HIDE);
    m_btnDis.ShowWindow(SW_SHOW);
    m_btnRec.ShowWindow(SW_SHOW);
    this->Invalidate();
}

void CMyWnd::OnBnClickedBtnRec()
{
#if (QGDebug == 1) 
    m_btnRec.EnableWindow(TRUE);
    m_btnRec.SetWindowTextW(_T("ʶ��"));
    m_btnRec.ShowWindow(SW_HIDE);
    m_btnDis.ShowWindow(SW_HIDE);
    ScaleWood scalewood;
    scalewood.id = time(0);
    scalewood.img = "";
    WoodAttr woodAttr1 = { 0 };
    woodAttr1.ellipse = { 2436.7294921875000, 963.02606201171875, 54.000000000000000, 57.000000000000000, 63.426303863525391,
        2383.0000000000000, 984.00000000000000, 2489.0000000000000, 943.00000000000000, 
        2455.0000000000000, 1013.0000000000000, 2417.0000000000000, 912.00000000000000 };
    woodAttr1.diameter = 13.400000000000000;
    woodAttr1.diameters = { 13.400000000000000, 14.300000000000001 };
    woodAttr1.volumn = 0.0000000000000000;
    
    WoodAttr woodAttr2 = { 0 };
    woodAttr2.ellipse = { 2258.6398925781250, 1551.4863281250000, 56.000000000000000, 57.000000000000000, 124.72299957275391,
        2231.0000000000000, 1502.0000000000000, 2285.0000000000000, 1602.0000000000000,
        2210.0000000000000, 1578.0000000000000, 2307.0000000000000, 1527.0000000000000 };
    woodAttr2.diameter = 5.0000000000000000;
    woodAttr2.diameters = { 5.0000000000000000, 5.2000000000000002 };
    woodAttr2.volumn = 0.0000000000000000;

    WoodAttr woodAttr3 = { 0 };
    woodAttr3.ellipse = { 2189.5292968750000, 1670.6750488281250, 73.000000000000000, 79.000000000000000, 120.99822998046875,
        2120.0000000000000, 1635.0000000000000, 2260.0000000000000, 1706.0000000000000,
        2158.0000000000000, 1735.0000000000000, 2222.0000000000000, 1606.0000000000000 };
    woodAttr3.diameter = 6.5999999999999996;
    woodAttr3.diameters = { 6.5999999999999996, 7.2000000000000002 };
    woodAttr3.volumn = 0.0000000000000000;

    WoodAttr woodAttr4 = { 0 };
    woodAttr4.ellipse = { 2362.7504882812500, 1676.9362792968750, 65.000000000000000, 71.000000000000000, 52.447589874267578,
        2306.0000000000000, 1720.0000000000000, 2417.0000000000000, 1632.0000000000000,
        2403.0000000000000, 1726.0000000000000, 2320.0000000000000, 1625.0000000000000 };
    woodAttr4.diameter = 6.0000000000000000;
    woodAttr4.diameters = { 6.0000000000000000, 6.5000000000000000 };
    woodAttr4.volumn = 0.0000000000000000;

    scalewood.wood_list.push_back(woodAttr1);
    scalewood.wood_list.push_back(woodAttr2);
    scalewood.wood_list.push_back(woodAttr3);
    scalewood.wood_list.push_back(woodAttr4);

#else
    CWaitCursor wait;
    m_btnRec.SetWindowTextW(_T("ʶ����"));
    m_btnRec.EnableWindow(FALSE);
    m_btnDis.ShowWindow(SW_HIDE);
    ScaleWood scalewood = { 0 };
    int ret = PostScale(scalewood);
    scalewood.id = time(0);
    m_btnRec.EnableWindow(TRUE);
    m_btnRec.SetWindowTextW(_T("ʶ��"));
    wait.Restore();
    if (ret < 0)
    {
        m_btnRec.ShowWindow(SW_SHOWNORMAL);
        m_btnDis.ShowWindow(SW_SHOWNORMAL);
        AfxMessageBox(_T("ʶ��ʧ�ܣ�������"));
        return;
    }
    else
    {
        m_btnRec.ShowWindow(SW_HIDE);
    }
    try
    {
        scalewood.img = base64_decode(scalewood.img);
    }
    catch (const std::exception&)
    {
        WriteLog(_T("invalid base64"));
        m_btnRec.ShowWindow(SW_SHOWNORMAL);
        m_btnDis.ShowWindow(SW_SHOWNORMAL);
        AfxMessageBox(_T("ʶ��ʧ�ܣ�������"));
        return;
    }
    if (scalewood.img.length() <= 0)
    {
        m_btnRec.ShowWindow(SW_SHOWNORMAL);
        m_btnDis.ShowWindow(SW_SHOWNORMAL);
        AfxMessageBox(_T("ʶ��ʧ�ܣ�������"));
        return;
    }
    //TODO limg,base64���ܣ��汾���ļ�����תΪIStream
    std::vector<uchar> img_data(scalewood.img.begin(), scalewood.img.end());
    cv::Mat img = cv::imdecode(cv::Mat(img_data), cv::IMREAD_COLOR);
    //cv::imwrite("..\\Doc\\img.jpg", img);
    std::string imagePath = GetImagePathUTF8() + std::to_string(scalewood.id) + ".jpg";
    cv::imwrite(imagePath, img);
#endif
    std::string strImagePath = GetImagePathUTF8() + "img.jpg";
    cv::Mat img = cv::imread(strImagePath);
    strImagePath = GetImagePathUTF8() + std::to_string(scalewood.id) + ".jpg";
    cv::imwrite(strImagePath, img);

    m_image.Destroy();
    CString imagePath;
    imagePath.Format(_T("%s%d.jpg"), GetImagePath(), scalewood.id);
    m_image.Load(imagePath); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��
    //AfxMessageBox(_T("����ʶ��ӿ�"));
    m_scaleWood = scalewood;
    SetStatus(0);
    this->Invalidate();
}

void CMyWnd::OnBnClickedBtnDis()
{
    //AfxMessageBox(_T("����")); 
    m_image.Destroy();
    m_btnCapture.ShowWindow(SW_SHOW);
    m_btnDis.ShowWindow(SW_HIDE);
    m_btnRec.ShowWindow(SW_HIDE);
    this->Invalidate();
}

void CMyWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    //����Ƿ���ɾ�����İ���
    if (nChar == VK_DELETE && m_status == 2)
    {
        bool isDelete = false;
        auto iter = m_scaleWood.wood_list.begin();
        while (iter != m_scaleWood.wood_list.end())
        {
            if (iter->isDeleting)
            {
                iter = m_scaleWood.wood_list.erase(iter);
                isDelete = true;
            }
            else
            {
                iter++;
            }
        }
        if (isDelete)
        {
            m_image.Destroy();
            //m_image.Load(_T("..\\Doc\\img.jpg"));
            CString imagePath;
            imagePath.Format(_T("%s%d.jpg"), GetImagePath(), m_scaleWood.id);
            m_image.Load(imagePath); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��

            Invalidate();
        }
    }
    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CMyWnd::ResetCapture()
{ 
    m_image.Destroy();
    m_btnCapture.ShowWindow(SW_SHOW);
    m_btnDis.ShowWindow(SW_HIDE);
    m_btnRec.ShowWindow(SW_HIDE);

    m_points.clear();

    m_ptOffset = CPoint();
    m_bDragging = FALSE;
    m_bDbClick = FALSE;
    m_ptLastMousePos = CPoint();

    m_status = 0;
    m_isPolygonComplete = false;

    m_startPoint = CPoint();
    m_endPoint = CPoint();

    m_isDrawFinished = false;
    m_scaleWood = { 0 };
    m_ellipse_add = { 0 };

    this->Invalidate();
}

void CMyWnd::ShowHistoryData(ScaleWood* pScaleWood)
{
    ResetCapture();

    m_scaleWood.id = pScaleWood->id;
    m_scaleWood.img = pScaleWood->img;
    m_scaleWood.wood_list = pScaleWood->wood_list;

    //std::string strImagePath = GetImagePathUTF8() + std::to_string(m_scaleWood.id) + ".jpg";
    //cv::Mat img = cv::imread(strImagePath);
   
    //m_image.Destroy();
    CString imagePath;
    imagePath.Format(_T("%s%d.jpg"), GetImagePath(), m_scaleWood.id);
    m_image.Load(imagePath); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��
    SetStatus(0);
    m_btnCapture.ShowWindow(SW_HIDE);
    m_btnDis.ShowWindow(SW_HIDE);
    m_btnRec.ShowWindow(SW_HIDE);
    this->Invalidate();
}