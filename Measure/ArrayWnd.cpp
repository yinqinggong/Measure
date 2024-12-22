#include "pch.h"
#include "ArrayWnd.h"
#include <iostream>
#include "CDlgDiameter.h"
#include "base64.h"
#include "LogFile.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "common.h"
#include "WoodEditDlg.h"

#if (CloudAPI == 1 && QGDebug == 1)
#include <fstream>
#include <string>
#include <sstream>
#endif


#ifndef M_PI
#define M_PI   3.141592653589793238462643383279502884
#endif

#define IDC_BTN_CAPTURE                 8000+1
#define IDC_BTN_REC                     8000+2
#define IDC_BTN_DIS                     8000+3

BEGIN_MESSAGE_MAP(CArrayWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    //ON_BN_CLICKED(IDC_BTN_CAPTURE, &CArrayWnd::OnBnClickedBtnCapture)
    //ON_BN_CLICKED(IDC_BTN_REC, &CArrayWnd::OnBnClickedBtnRec)
    //ON_BN_CLICKED(IDC_BTN_DIS, &CArrayWnd::OnBnClickedBtnDis)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CArrayWnd::CArrayWnd()
	: m_scaleFactor(1.0f)
	, m_imageOrigin(0, 0)
	, m_dragging(false)
	, m_status(0)
    , m_bRun(false)
    , m_workStatus(0)
    , m_wndIndex(-1)
    , m_share_wood_id(0)
{
    m_scaleWood = { 0 };
    m_ellipse_add = { 0 };
}

CArrayWnd::~CArrayWnd()
{
    StopThread();
}

int CArrayWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    //m_btnCapture.Create(_T("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_CAPTURE);
    //m_btnRec.Create(_T("ʶ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_REC);
    //m_btnDis.Create(_T("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_DIS);

    //CRect rect;
    //GetClientRect(&rect);
    //m_btnCapture.MoveWindow(rect.right - 120, rect.Height() * 0.5, 100, 50);
    //m_btnRec.MoveWindow(rect.right - 120, rect.Height() * 0.5 - 100, 100, 50);
    //m_btnDis.MoveWindow(rect.right - 120, rect.Height() * 0.5 + 100, 100, 50);

    StartThread();
    return 0;
}
BOOL CArrayWnd::LoadLocalImage(LPCTSTR lpszPath, bool firstInit)
{
    HRESULT hr = m_image.Load(lpszPath);
    if (FAILED(hr))
    {
        AfxMessageBox(_T("Failed to load image"));
        return FALSE;
    }
    if (firstInit)
    {
        //��������֮�󣬳�ʼ���ŵ���С
        CRect clientRect;
        GetClientRect(&clientRect);
        float minScaleX = static_cast<float>(clientRect.Width()) / m_image.GetWidth();
        float minScaleY = static_cast<float>(clientRect.Height()) / m_image.GetHeight();
        m_scaleFactor = std::max(minScaleX, minScaleY);
    }

    Invalidate();
    return TRUE;
}

void CArrayWnd::DrawRotatedEllipse(Gdiplus::Graphics* graphics, WoodAttr& woodAttr)
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
    Gdiplus::Font font(&fontFamily, 30, FontStyleRegular, UnitPixel);
    Gdiplus::PointF pointF(static_cast<float>(woodAttr.ellipse.cx - woodAttr.ellipse.ab1 * 0.5), static_cast<float>(woodAttr.ellipse.cy - woodAttr.ellipse.ab2 * 0.3));
    Gdiplus::SolidBrush solidBrush(Gdiplus::Color(255, 255, 0, 0)); // ��ɫ��ˢ
    graphics->DrawString(text, -1, &font, pointF, &solidBrush);

    // �ָ�ͼ��״̬����ȷ����������ͼ�β���Ӱ��
   // Matrix matrix0;
    //graphics->SetTransform(&matrix0); // ���ñ任����
}

void CArrayWnd::DrawRotatedEllipse(Gdiplus::Graphics* graphics, Gdiplus::Pen& pen, int cx, int cy, int ab1, int ab2, double angle)
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

void CArrayWnd::DrawPolygon(Gdiplus::Graphics* graphics, PointF points[], int numPoints)
{
    Pen red_pen(Gdiplus::Color(255, 255, 0, 0), 2);
    graphics->DrawPolygon(&red_pen, points, numPoints);
}

void CArrayWnd::OnPaint()
{
    CPaintDC dc(this);
    CRect clientRect;
    GetClientRect(&clientRect);

	if (m_image.IsNull())
	{
		dc.FillSolidRect(clientRect, RGB(42, 42, 43));   //�ؼ�����ɫ
		//m_btnCapture.ShowWindow(SW_SHOW);
		//m_btnDis.ShowWindow(SW_HIDE);
		//m_btnRec.ShowWindow(SW_HIDE);
        return;
	}

    /*ʹ��˫���壬����ͬʱ���Ʊ�����ͼƬʱ������˸*/
    CDC memDC; // �ڴ��е��豸������
    CBitmap memBitmap; // �ڴ��е�λͼ
    memBitmap.CreateCompatibleBitmap(&dc, clientRect.Width(), clientRect.Height());// ����һ���봰�ڴ�С��ͬ��λͼ
    memDC.CreateCompatibleDC(&dc);
    CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

    //����ɫ����ͼƬ����С�ڴ����ǣ���Ҫ����ɫ
    memDC.FillSolidRect(clientRect, RGB(42, 42, 43));   //�ؼ�����ɫ

    // ����ƽ������ģʽ
    memDC.SetStretchBltMode(HALFTONE);
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
    //m_image.Draw(dc, destRect);

    if (!m_image.IsNull())
    {
        if (m_status == 0 || (m_status == 1 && m_isDrawFinished) || m_status == 2)
        {
            // ��ȡGDI+ͼ�ζ���
            Gdiplus::Graphics graphics(m_image.GetDC());

            for (size_t i = 0; i < m_scaleWood.wood_list.size(); i++)
            {
                DrawRotatedEllipse(&graphics, m_scaleWood.wood_list[i]);
            }
            // �ͷ�GDI+ͼ�ζ���
            m_image.ReleaseDC();

            //��Բ���Ƴɹ����ٻ�����ͼƬ
            //::SetStretchBltMode(dc, HALFTONE);
            //::SetBrushOrgEx(dc, 0, 0, NULL);
            m_image.Draw(memDC, destRect);
        }
        else if (m_status == 1)
        {
            //::SetStretchBltMode(dc, HALFTONE);
            //::SetBrushOrgEx(dc, 0, 0, NULL);
            m_image.Draw(memDC, destRect);
            // ��ȡGDI+ͼ�ζ���
            Gdiplus::Graphics graphics(memDC.GetSafeHdc());
            // ����һ������
            Pen pen(Gdiplus::Color(255, 255, 255, 255), 2);
            int cx = (m_startPoint.x + m_endPoint.x) * 0.5;
            int cy = (m_startPoint.y + m_endPoint.y) * 0.5;
            int ab1 = abs(m_endPoint.x - m_startPoint.x) * 0.5;
            int ab2 = abs(m_endPoint.y - m_startPoint.y) * 0.5;
            double angel = CalculateAngle(m_startPoint.x, m_startPoint.y, m_endPoint.x, m_endPoint.y);
            DrawRotatedEllipse(&graphics, pen, cx, cy, ab1, ab2, angel);
        }
        else if (m_status == 3)
        {
            //::SetStretchBltMode(dc, HALFTONE);
            //::SetBrushOrgEx(dc, 0, 0, NULL);
            m_image.Draw(memDC, destRect);
            // ����͸������ģʽ
            memDC.SetBkMode(TRANSPARENT);
            // �������ʺͻ�ˢ
            CPen pen(PS_SOLID, 2, RGB(255, 255, 255)); // ��ɫ�߿�
            CBrush* pOldBrush = static_cast<CBrush*>(memDC.SelectStockObject(NULL_BRUSH)); // �ջ�ˢ
            CPen* pOldPen = memDC.SelectObject(&pen);

            // ���ƶ����
            if (m_points.size() > 1)
            {
                for (size_t i = 0; i < m_points.size() - 1; i++)
                {
                    //��ת��Ϊ��Ļ�����ٻ���
                    memDC.MoveTo(ImageToScreen(m_points[i]));
                    memDC.LineTo(ImageToScreen(m_points[i + 1]));
                }
            }
            // �ָ�ԭ���Ļ��ʺͻ�ˢ
            memDC.SelectObject(pOldPen);
            memDC.SelectObject(pOldBrush);
        }
    }

    /*˫���弼������*/
    // ���ڴ��е�λͼһ���Ը��Ƶ���Ļ��
    dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
    // �ָ�ԭ����λͼ
    memDC.SelectObject(pOldBitmap);
}

BOOL CArrayWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

void CArrayWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    //���ж�ʶ��ɹ����ٵ���
    CString imagePath;
    imagePath.Format(_T("%s%d_%d.jpg"), GetImagePath(), m_scaleWood.id, m_wndIndex);
    if (!FileExistW(imagePath))
    {
        AfxMessageBox(_T("����ʶ��ľ�ģ�"));
        return;
    }

    CWoodEditDlg woodEditDlg;
    woodEditDlg.SetWndIndex(m_wndIndex);
    ScaleWood* pScaleWood = new ScaleWood(m_scaleWood);
    woodEditDlg.SetScaleWood(pScaleWood);
    woodEditDlg.DoModal();
    if (pScaleWood->img.length() > 0)
    {
        pScaleWood->img = "";
        m_scaleWood = *pScaleWood; 
        m_image.Destroy();
        LoadLocalImage(imagePath, false);
        Invalidate();
    }
    delete pScaleWood;
    pScaleWood = NULL;
    CWnd::OnLButtonDown(nFlags, point);
    return;
    if (!m_image.IsNull())
    {
        if (m_status == 0)
        {
            m_dragging = true;
            m_lastMousePos = point;
            SetCapture(); // ������꣬�����϶��¼�
        }
        else if (m_status == 1)
        {
            // ��¼��ʼ��
            m_isDrawFinished = false;
            m_startPoint = point;
        }
        else if (m_status == 2)
        {
            m_dragging = true;
            m_lastMousePos = point;
			SetCapture(); // ������꣬�����϶��¼�

			//�жϵ�ǰ���Ƿ���ĳ����Բ�ڲ�
			CPoint curPoint = ScreenToImage(point);
			if (isPointInEllipse(curPoint.x, curPoint.y))
			{
				SetFocus();
				Invalidate();
			}
        }
        else if (m_status == 3)
        {
            if (IsShiftKeyDown())
            {
                m_dragging = true;
                m_lastMousePos = point;
                SetCapture(); // ������꣬�����϶��¼�
            }
            else
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
                        //ע�ʹ˴���ת����m_points�޸�Ϊһֱ�������ͼƬ���꣬
                        //��ֹͼƬ�Ŵ����Сʱ������Ӧ������ͼƬʱ����ת������Ļ����
                        /*for (size_t i = 0; i < m_points.size(); i++)
                        {
                            m_points[i] = ScreenToImage(m_points[i]);
                        }*/

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
                        imagePath.Format(_T("%s%d_%d.jpg"), GetImagePath(), m_scaleWood.id, m_wndIndex);
                        LoadLocalImage(imagePath, false);
                        SetStatus(0);
                        ::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
                        m_points.clear();
                    }
                }
                else
                {
                    CPoint imgPoint = ScreenToImage(point);
                    m_points.push_back(imgPoint);
                }
            }

            Invalidate();
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void CArrayWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_status == 0 || m_status == 2 || (m_status == 3 /*&& IsShiftKeyDown()*/))
    {
        if (m_dragging)
        {
            m_dragging = false;
            ReleaseCapture();
        }
    }
    else if (m_status == 1)
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
        SetStatus(0);
        ::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
        m_isDrawFinished = true;
        Invalidate();
    }

    CWnd::OnLButtonUp(nFlags, point);
}

void CArrayWnd::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_status == 0 || m_status == 2 || (m_status == 3 && IsShiftKeyDown()))
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
    }
    else if (m_status == 1)
    {
        // ������������£����������ƶ������½����㲢�ػ�
        if ((nFlags & MK_LBUTTON) && (m_startPoint != point))
        {
			// ���½�����
			m_endPoint = point;
			//�϶�֮��Ҫ���¼�����ʼ��
			CPoint newStartPoint = ScreenToImage(m_startPoint);
			CPoint newEndPoint = ScreenToImage(m_endPoint);
			m_ellipse_add.cx = (newStartPoint.x + newEndPoint.x) * 0.5;
			m_ellipse_add.cy = (newStartPoint.y + newEndPoint.y) * 0.5;
			m_ellipse_add.ab1 = abs(newEndPoint.x - newStartPoint.x) * 0.5;
			m_ellipse_add.ab2 = abs(newEndPoint.y - newStartPoint.y) * 0.5;
			m_ellipse_add.angel = CalculateAngle(newStartPoint.x, newStartPoint.y, newEndPoint.x, newEndPoint.y);

			Invalidate();
        }
    }
    else if (m_status == 3 && !IsShiftKeyDown())
    {
        //��갴�µ�һ���������ƶ��ȼ���ڶ�����
        if (m_points.size() == 1)
        {
            CPoint imgPoint = ScreenToImage(point);
            m_points.push_back(imgPoint);
        }
        //��������ƶ��������޸����һ����
        if (m_points.size() > 1)
        {
            CPoint imgPoint = ScreenToImage(point);
            m_points[m_points.size() - 1].SetPoint(imgPoint.x, imgPoint.y);
        }

        Invalidate();
    }

    CWnd::OnMouseMove(nFlags, point);
}

void CArrayWnd::AdjustImageOrigin()
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

void CArrayWnd::LimitScalingFactor()
{
    CRect clientRect;
    GetClientRect(&clientRect);

    // �����������ӣ���ֹͼ����С���ȴ��ڻ�С
    float minScaleX = static_cast<float>(clientRect.Width()) / m_image.GetWidth();
    float minScaleY = static_cast<float>(clientRect.Height()) / m_image.GetHeight();
    float minScale = std::max(minScaleX/2, minScaleY/2);

    m_scaleFactor = std::max(m_scaleFactor, minScale);
    m_scaleFactor = std::min(m_scaleFactor, 10.0f); // Ҳ��������������ű���
}

bool CArrayWnd::isCloseEnough(const CPoint& p1, const CPoint& p2, int threshold)
{
    //�Ȱѵ�һ����תΪ��Ļ��
    CPoint screenPoint = ImageToScreen(p1);
    int dx = screenPoint.x - p2.x;
    int dy = screenPoint.y - p2.y;
    return (dx * dx + dy * dy) <= (threshold * threshold);
}

bool CArrayWnd::isPointInEllipse(const CPoint& p)
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

bool CArrayWnd::isPointInEllipse(int px, int py)
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

bool CArrayWnd::IsPointInEllipse(int px, int py, int cx, int cy, int a, int b, double angle)
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

bool CArrayWnd::IsPointInPolygon(int px, int py, const std::vector<CPoint>& polygon)
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

double CArrayWnd::CalculateAngle(int startX, int startY, int endX, int endY)
{
    //����Ҫ���
    return 0.0;
    // ��������֮��ĽǶȣ��Զ�Ϊ��λ��
    double dx = endX - startX;
    double dy = endY - startY;
    double angle = atan2(dy, dx) * 180.0 / M_PI;

    return angle;
}
//
//void CArrayWnd::OnBnClickedBtnCapture()
//{
//}
//
//void CArrayWnd::OnBnClickedBtnRec()
//{
//    m_evt_beginRecEvent.SetEvent();
//}
//
//void CArrayWnd::OnBnClickedBtnDis()
//{
//    m_image.Destroy();
//    m_btnCapture.ShowWindow(SW_SHOW);
//    m_btnDis.ShowWindow(SW_HIDE);
//    m_btnRec.ShowWindow(SW_HIDE);
//    this->Invalidate();
//}

void CArrayWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
            CString imagePath;
            imagePath.Format(_T("%s%d_%d.jpg"), GetImagePath(), m_scaleWood.id, m_wndIndex);
            LoadLocalImage(imagePath, false);
            Invalidate();
        }
    }
    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CArrayWnd::ResetCapture()
{
    m_image.Destroy();
    //m_btnCapture.ShowWindow(SW_SHOW);
    //m_btnDis.ShowWindow(SW_HIDE);
    //m_btnRec.ShowWindow(SW_HIDE);

    m_points.clear();

    m_dragging = false;
    m_lastMousePos = CPoint();
    m_imageOrigin = CPoint();
    m_status = 0;
    m_scaleFactor = 1.0;
    m_startPoint = CPoint();
    m_endPoint = CPoint();

    m_isDrawFinished = false;
    m_scaleWood = { 0 };
    m_ellipse_add = { 0 };
    m_share_wood_id = 0;
    m_workStatus = 0;

    this->Invalidate();
}


void CArrayWnd::ShowHistoryData(ScaleWood* pScaleWood)
{
    ResetCapture();

    m_scaleWood.id = pScaleWood->id;
    m_scaleWood.img = pScaleWood->img;
    m_scaleWood.wood_list = pScaleWood->wood_list;

    CString imagePath;
    imagePath.Format(_T("%s%d_%d.jpg"), GetImagePath(), m_scaleWood.id, m_wndIndex);
    //m_image.Load(imagePath); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��
    LoadLocalImage(imagePath, true);
    SetStatus(0);
    ::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
    //m_btnCapture.ShowWindow(SW_HIDE);
    //m_btnDis.ShowWindow(SW_HIDE);
    //m_btnRec.ShowWindow(SW_HIDE);
    this->Invalidate();
}

CPoint CArrayWnd::ScreenToImage(CPoint screenPoint)
{
    // ����Ļ����ת��Ϊ���ڵĿͻ�������
    //ScreenToClient(&screenPoint);

    // �������ź�ͼƬ��ԭ��λ�ã����ͻ�������ת��ΪͼƬ����
    CPoint imagePoint;
    imagePoint.x = static_cast<int>((screenPoint.x - m_imageOrigin.x) / m_scaleFactor);
    imagePoint.y = static_cast<int>((screenPoint.y - m_imageOrigin.y) / m_scaleFactor);

    return imagePoint;
}

CPoint CArrayWnd::ImageToScreen(CPoint imagePoint)
{
    // ����Ļ����ת��Ϊ���ڵĿͻ�������
    //ScreenToClient(&screenPoint);

    // �������ź�ͼƬ��ԭ��λ�ã����ͻ�������ת��ΪͼƬ����
    CPoint screenPoint;
    screenPoint.x = static_cast<int>(imagePoint.x * m_scaleFactor + m_imageOrigin.x);
    screenPoint.y = static_cast<int>(imagePoint.y * m_scaleFactor + m_imageOrigin.y);

    return screenPoint;
}

bool CArrayWnd::StartThread()
{
    //���������߳�
    m_bRun = true;
    m_recThread = AfxBeginThread(RecThread, (LPVOID)this);
    if (m_recThread == NULL)
    {
        m_bRun = false;
        return false;
    }
    m_hRecThreadHandle = m_recThread->m_hThread;
    //m_evt_checkRecordEvent.SetEvent();
    return true;
}


UINT CArrayWnd::RecThread(LPVOID lpParam)
{
    CArrayWnd* pDecode = (CArrayWnd*)lpParam;
    if (!pDecode)
    {
        return 0;
    }
    while (WaitForSingleObject(pDecode->m_evt_beginRecEvent, INFINITE) == WAIT_OBJECT_0)
    {
        if (pDecode->m_bRun == false) break;
        int workStatus = pDecode->GetWorkStatus();
        if (workStatus == 1)
        {
            pDecode->PhotoMethod();
            pDecode->SetWorkStatus(2);
        }
        else if(workStatus == 3)
        {
            pDecode->RecMethod();
            pDecode->SetWorkStatus(4);
        }
        else
        {
            
        }
        if (pDecode->m_bRun == false) break;
    }

    return 1;
}

bool CArrayWnd::StopThread()
{
    m_workStatus = 0;
    m_bRun = false;
    m_evt_beginRecEvent.SetEvent();
    if (m_hRecThreadHandle != INVALID_HANDLE_VALUE)
    {
        WaitAndTermThread(m_hRecThreadHandle, 10000);
        m_hRecThreadHandle = INVALID_HANDLE_VALUE;
        m_recThread = NULL;
    }
    return true;
}
void CArrayWnd::PhotoMethod()
{
#if (QGDebug == 1)
    ShowWindow(SW_SHOW);
#else
    ShowWindow(SW_SHOW);
    std::string limg;
    CWaitCursor wait;
    int errorCode = 0;
    int ret = PostPhoto(m_limg, errorCode, m_rimg, m_camparam);
    wait.Restore();
    if (ret < 0)
    {
        WriteLog(_T("PostPhoto%d API failed, errorCode:%d"), m_wndIndex + 1, errorCode);
        CString tipStr;
        tipStr.Format(_T("���%d����ʧ�ܣ�������, code:%d"), m_wndIndex + 1, errorCode);
        AfxMessageBox(tipStr);
        return;
    }
    try
    {
        limg = m_limg;
        limg = base64_decode(limg);
    }
    catch (const std::exception&)
    {
        WriteLog(_T("invalid base64 exception"));
        AfxMessageBox(_T("��ȡͼƬʧ�ܣ�������"));
        return;
    }
    if (limg.length() <= 0)
    {
        WriteLog(_T("invalid base64 limg.length() <= 0"));
        AfxMessageBox(_T("��ȡͼƬʧ�ܣ�������"));
        return;
    }

    std::vector<uchar> img_data(limg.begin(), limg.end());
    cv::Mat img = cv::imdecode(cv::Mat(img_data), cv::IMREAD_COLOR);
    cv::imwrite(GetImagePathUTF8() + "limg_" +std::to_string(m_wndIndex) +".jpg", img);
#endif

#if (CloudAPI == 1 && QGDebug == 1)
    /*std::ifstream file(GetImagePathUTF8() + "stereo_params.xml");
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    m_cam_params = buffer.str();*/

    m_camparam = "[[2685.4126058357715,0.0,1938.4374318654877,0.0,2685.4575283727913,1562.856538949121,0.0,0.0,1.0],[-0.09367156152199602,0.11420308542792396,-0.0003192966334184126,-0.0003126638912737841,-0.03576168360979528],[2694.5055745713976,0.0,2016.023715727735,0.0,2694.5334647064997,1624.3703386253874,0.0,0.0,1.0],[-0.0990762798801572,0.1344623330667149,0.0002629763856103351,-0.00026026834521232495,-0.06171569320487198],[0.9999947340940366,0.00021165684138245476,0.0032383615577255775,-0.00020970907983761957,0.9999997969337054,-0.0006017928628442911,-0.003238488273700012,0.0006011105800372101,0.9999945754151728],[-0.17971870265605125,2.0874208450081025e-05,0.0007972581123076216],[9.959138583675177e-08,-0.0007972454027038217,2.1353879457998024e-05,0.00021523700290517237,0.00010819955873093142,0.17972030956672616,1.6814545235148994e-05,-0.17971867057940927,0.00010808583434385429],[-3.5409179639913875e-12,2.8345155772186532e-08,-4.6331382348966894e-05,-7.652556243705983e-09,-3.8468727340391154e-09,-0.017138383094346336,1.0826862081080727e-05,0.01716616880998382,1.0],[0.9999992784749311,9.283999889650864e-05,-0.0011976770648645069,-9.320033363703872e-05,0.999999950413806,-0.00030080904803023704,0.001197649078364573,0.0003009204548910047,0.9999992375414918],[0.9999901537116226,-0.00011614819497435116,-0.004436100697916132,0.00011748285957754541,0.9999999479172159,0.00030060496215355463,0.004436065552147903,-0.0003011231681057728,0.9999901152747734],[2689.9954965396455,0.0,2003.1144714355469,0.0,0.0,2689.9954965396455,1578.137191772461,0.0,0.0,0.0,1.0,0.0],[2689.9954965396455,0.0,2003.1144714355469,-483.4472609498719,0.0,2689.9954965396455,1578.137191772461,0.0,0.0,0.0,1.0,0.0],[1.0,0.0,0.0,-2003.1144714355469,0.0,1.0,0.0,-1578.137191772461,0.0,0.0,0.0,2689.9954965396455,0.0,0.0,5.5641963742940055,0.0]]";

    std::ifstream file1(GetImagePathUTF8() + "l.jpg", std::ios::binary);
    if (!file1) {
        std::cerr << "Unable to open file" << std::endl;
        return;
    }
    std::ostringstream oss1;
    oss1 << file1.rdbuf();
    m_limg = base64_encode(oss1.str());

    std::ifstream file2(GetImagePathUTF8() + "r.jpg", std::ios::binary);
    if (!file2) {
        std::cerr << "Unable to open file" << std::endl;
        return;
    }
    std::ostringstream oss2;
    oss2 << file2.rdbuf();
    m_rimg = base64_encode(oss2.str());

    std::string limg;
    try
    {
        limg = base64_decode(m_limg);
    }
    catch (const std::exception&)
    {
        WriteLog(_T("invalid base64 exception"));
        AfxMessageBox(_T("��ȡͼƬʧ�ܣ�������"));
        return;
    }
    if (limg.length() <= 0)
    {
        WriteLog(_T("invalid base64 limg.length() <= 0"));
        AfxMessageBox(_T("��ȡͼƬʧ�ܣ�������"));
        return;
    }

    std::vector<uchar> img_data(limg.begin(), limg.end());
    cv::Mat img = cv::imdecode(cv::Mat(img_data), cv::IMREAD_COLOR);
    cv::imwrite(GetImagePathUTF8() + "limg_" + std::to_string(m_wndIndex) + ".jpg", img);
#endif
    CString limgName;
    limgName.Format(_T("limg_%d.jpg"), m_wndIndex);
    LoadLocalImage(GetImagePath() + limgName, true);
    //m_btnCapture.ShowWindow(SW_HIDE);
    //m_btnDis.ShowWindow(SW_SHOW);
    //m_btnRec.ShowWindow(SW_SHOW);
    this->Invalidate();
}

void CArrayWnd::RecMethod()
{
#if (QGDebug == 1 && CloudAPI == 0) 
    //m_btnRec.EnableWindow(TRUE);
    //m_btnRec.SetWindowTextW(_T("ʶ��"));
    //m_btnRec.ShowWindow(SW_HIDE);
    //m_btnDis.ShowWindow(SW_HIDE);
    ScaleWood scalewood;
    scalewood.id = m_share_wood_id;
    scalewood.img = "";
    WoodAttr woodAttr1 = { 0 };
    woodAttr1.ellipse = { 2436.7294921875000, 963.02606201171875, 54.000000000000000, 57.000000000000000, 63.426303863525391,
        2383.0000000000000, 984.00000000000000, 2489.0000000000000, 943.00000000000000,
        2455.0000000000000, 1013.0000000000000, 2417.0000000000000, 912.00000000000000 };
    woodAttr1.diameter = 13.400000000000000;
    woodAttr1.diameters = { 13.400000000000000, 14.300000000000001 };
    woodAttr1.volumn = 0.0000000000000000;
    woodAttr1.index = m_wndIndex;

    WoodAttr woodAttr2 = { 0 };
    woodAttr2.ellipse = { 2258.6398925781250, 1551.4863281250000, 56.000000000000000, 57.000000000000000, 124.72299957275391,
        2231.0000000000000, 1502.0000000000000, 2285.0000000000000, 1602.0000000000000,
        2210.0000000000000, 1578.0000000000000, 2307.0000000000000, 1527.0000000000000 };
    woodAttr2.diameter = 5.0000000000000000;
    woodAttr2.diameters = { 5.0000000000000000, 5.2000000000000002 };
    woodAttr2.volumn = 0.0000000000000000;
    woodAttr2.index = m_wndIndex;

    WoodAttr woodAttr3 = { 0 };
    woodAttr3.ellipse = { 2189.5292968750000, 1670.6750488281250, 73.000000000000000, 79.000000000000000, 120.99822998046875,
        2120.0000000000000, 1635.0000000000000, 2260.0000000000000, 1706.0000000000000,
        2158.0000000000000, 1735.0000000000000, 2222.0000000000000, 1606.0000000000000 };
    woodAttr3.diameter = 6.5999999999999996;
    woodAttr3.diameters = { 6.5999999999999996, 7.2000000000000002 };
    woodAttr3.volumn = 0.0000000000000000;
    woodAttr3.index = m_wndIndex;

    WoodAttr woodAttr4 = { 0 };
    woodAttr4.ellipse = { 2362.7504882812500, 1676.9362792968750, 65.000000000000000, 71.000000000000000, 52.447589874267578,
        2306.0000000000000, 1720.0000000000000, 2417.0000000000000, 1632.0000000000000,
        2403.0000000000000, 1726.0000000000000, 2320.0000000000000, 1625.0000000000000 };
    woodAttr4.diameter = 6.0000000000000000;
    woodAttr4.diameters = { 6.0000000000000000, 6.5000000000000000 };
    woodAttr4.volumn = 0.0000000000000000;
    woodAttr4.index = m_wndIndex;

    scalewood.wood_list.push_back(woodAttr1);
    scalewood.wood_list.push_back(woodAttr2);
    scalewood.wood_list.push_back(woodAttr3);
    scalewood.wood_list.push_back(woodAttr4);

    std::string strImagePath = GetImagePathUTF8() + "img_" + std::to_string(m_wndIndex) + ".jpg";
    cv::Mat img = cv::imread(strImagePath);
    strImagePath = GetImagePathUTF8() + std::to_string(scalewood.id) +"_" + std::to_string(m_wndIndex) + ".jpg";
    cv::imwrite(strImagePath, img);

#else
    CWaitCursor wait;
    //m_btnRec.SetWindowTextW(_T("ʶ����"));
    //m_btnRec.EnableWindow(FALSE);
    //m_btnDis.ShowWindow(SW_HIDE);
    int errorCode = 0;
    ScaleWood scalewood = { 0 };
#if CloudAPI
    int w = 0, h = 0, c = 0;
    int ret = PostInfer(scalewood, errorCode, m_limg, m_rimg, m_camparam, w, h, c, m_wndIndex);
#else
    int ret = PostScale(scalewood, errorCode, m_wndIndex);
#endif
    //m_limg.clear();
    //m_rimg.clear();
    //m_camparam.clear();
    scalewood.id = m_share_wood_id;
    //m_btnRec.EnableWindow(TRUE);
    //m_btnRec.SetWindowTextW(_T("ʶ��"));
    wait.Restore();
    if (ret < 0)
    {
        //m_btnRec.ShowWindow(SW_SHOWNORMAL);
        //m_btnDis.ShowWindow(SW_SHOWNORMAL);
        CString tipStr;
        tipStr.Format(_T("���%dʶ��ʧ�ܣ�������, code:%d"), m_wndIndex + 1, errorCode);
        AfxMessageBox(tipStr);
        return;
    }
    else
    {
        //m_btnRec.ShowWindow(SW_HIDE);
    }
    try
    {
        scalewood.img = base64_decode(scalewood.img);
    }
    catch (const std::exception&)
    {
        WriteLog(_T("invalid base64"));
        //m_btnRec.ShowWindow(SW_SHOWNORMAL);
        //m_btnDis.ShowWindow(SW_SHOWNORMAL);
        AfxMessageBox(_T("ʶ��ʧ�ܣ�������, invalid base64-1"));
        return;
    }
    if (scalewood.img.length() <= 0)
    {
        //m_btnRec.ShowWindow(SW_SHOWNORMAL);
        //m_btnDis.ShowWindow(SW_SHOWNORMAL);
        AfxMessageBox(_T("ʶ��ʧ�ܣ�������, invalid base64-2"));
        return;
    }
#endif

#if CloudAPI
    cv::Mat img(w, h, CV_8UC3, (char*)scalewood.img.c_str());
#else
    /*std::vector<uchar> img_data(scalewood.img.begin(), scalewood.img.end());
    cv::Mat img = cv::imdecode(cv::Mat(img_data), cv::IMREAD_COLOR);*/
#endif
    /*if (img.empty())
    {
        AfxMessageBox(_T("ʶ��ʧ�ܣ�������, invalid base64-3"));
        return;
    }
    std::string imagePath = GetImagePathUTF8() + std::to_string(scalewood.id) + ".jpg";
    cv::imwrite(imagePath, img);*/

    m_image.Destroy();
    CString strImagePathW;
    strImagePathW.Format(_T("%s%d_%d.jpg"), GetImagePath(), scalewood.id, m_wndIndex);
    LoadLocalImage(strImagePathW, true);
    m_scaleWood = scalewood;
    SetStatus(0);
    ::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
    this->Invalidate();
}
