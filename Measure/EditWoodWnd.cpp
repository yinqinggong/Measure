#include "pch.h"
#include "EditWoodWnd.h"
#include <iostream>
#include "CDlgDiameter.h"
#include "base64.h"
#include "LogFile.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "common.h"

#if (CloudAPI == 1 && QGDebug == 1)
#include <fstream>
#include <string>
#include <sstream>
#endif


#ifndef M_PI
#define M_PI   3.141592653589793238462643383279502884
#endif

#define IDC_BTN_ADD                     8200+1
#define IDC_BTN_CROP                    8200+2
#define IDC_BTN_DEL                     8200+3
#define IDC_BTN_SAVE                    8200+4

BEGIN_MESSAGE_MAP(CEditWoodWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_BTN_ADD, &CEditWoodWnd::OnBnClickedBtnAdd)
    ON_BN_CLICKED(IDC_BTN_CROP, &CEditWoodWnd::OnBnClickedBtnCrop)
    ON_BN_CLICKED(IDC_BTN_DEL, &CEditWoodWnd::OnBnClickedBtnDel)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CEditWoodWnd::OnBnClickedBtnSave)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CEditWoodWnd::CEditWoodWnd()
	: m_scaleFactor(1.0f)
	, m_imageOrigin(0, 0)
	, m_dragging(false)
	, m_status(0)
    , m_pScaleWood(NULL)
    //, m_bRun(false)
    //, m_recing(false)
{
    m_ellipse_add = { 0 };
}

CEditWoodWnd::~CEditWoodWnd()
{
    //StopThread();
}

int CEditWoodWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    m_btnAdd.Create(_T("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_ADD);
    m_btnCrop.Create(_T("�ü�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_CROP);
    m_btnDel.Create(_T("ɾ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_DEL);
    m_btnSave.Create(_T("ȷ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_SAVE);

    CRect rect;
    GetClientRect(&rect);
    m_btnAdd.MoveWindow(rect.right - 120, rect.Height() * 0.5, 80, 40);
    m_btnCrop.MoveWindow(rect.right - 120, rect.Height() * 0.5 - 80, 80, 40);
    m_btnDel.MoveWindow(rect.right - 120, rect.Height() * 0.5 + 80, 80, 40);
    m_btnSave.MoveWindow(rect.right - 120, rect.Height() * 0.5 + 160, 80, 40);

    m_btnAdd.ModifyStyle(NULL, BS_OWNERDRAW);
    m_btnCrop.ModifyStyle(NULL, BS_OWNERDRAW);
    m_btnDel.ModifyStyle(NULL, BS_OWNERDRAW);
    m_btnSave.ModifyStyle(NULL, BS_OWNERDRAW);

    //StartThread();
    return 0;
}


BOOL CEditWoodWnd::LoadLocalImage(bool firstInit)
{
    CString imagePath;
    imagePath.Format(_T("%s%d_%d.jpg"), GetImagePath(), m_pScaleWood->id, m_wndIndex);
    return LoadLocalImage(imagePath, firstInit);
}
BOOL CEditWoodWnd::LoadLocalImage(LPCTSTR lpszPath, bool firstInit)
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

void CEditWoodWnd::DrawRotatedEllipse(Gdiplus::Graphics* graphics, WoodAttr& woodAttr)
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

void CEditWoodWnd::DrawRotatedEllipse(Gdiplus::Graphics* graphics, Gdiplus::Pen& pen, int cx, int cy, int ab1, int ab2, double angle)
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

void CEditWoodWnd::DrawPolygon(Gdiplus::Graphics* graphics, PointF points[], int numPoints)
{
    Pen red_pen(Gdiplus::Color(255, 255, 0, 0), 2);
    graphics->DrawPolygon(&red_pen, points, numPoints);
}

void CEditWoodWnd::OnPaint()
{
    CPaintDC dc(this);
    CRect clientRect;
    GetClientRect(&clientRect);

	if (m_image.IsNull())
	{
		dc.FillSolidRect(clientRect, RGB(42, 42, 43));   //�ؼ�����ɫ
		//m_btnAdd.ShowWindow(SW_SHOW);
		//m_btnDel.ShowWindow(SW_HIDE);
		//m_btnCrop.ShowWindow(SW_HIDE);
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

            for (size_t i = 0; i < m_pScaleWood->wood_list.size(); i++)
            {
                DrawRotatedEllipse(&graphics, m_pScaleWood->wood_list[i]);
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

BOOL CEditWoodWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

void CEditWoodWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
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
                        for (size_t i = 0; i < m_pScaleWood->wood_list.size(); i++)
                        {
                            if (IsPointInPolygon(m_pScaleWood->wood_list[i].ellipse.cx,
                                m_pScaleWood->wood_list[i].ellipse.cy, m_points))
                            {
                                temp_wood_list.push_back(m_pScaleWood->wood_list[i]);
                            }
                        }
                        m_pScaleWood->wood_list = temp_wood_list;
                        //֮ǰ�Ļ��Ƶ���Բ��Ч�ˣ���Ҫ���»���
                        m_image.Destroy();
                        CString imagePath;
                        imagePath.Format(_T("%s%d_%d.jpg"), GetImagePath(), m_pScaleWood->id, m_wndIndex);
                        LoadLocalImage(imagePath, false);
                        SetStatus(0);
                        ResetBtnBgColor();
                        //::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
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

void CEditWoodWnd::OnLButtonUp(UINT nFlags, CPoint point)
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
                    woodAttr.index = m_wndIndex;
                    woodAttr.diameter = d;
                    woodAttr.ellipse = m_ellipse_add;
                    woodAttr.diameters.d1 = m_ellipse_add.ab1;
                    woodAttr.diameters.d2 = m_ellipse_add.ab2;
                    m_pScaleWood->wood_list.push_back(woodAttr);
                }
            }
        }
        SetStatus(0);
        ResetBtnBgColor();
        //::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
        m_isDrawFinished = true;
        Invalidate();
    }

    CWnd::OnLButtonUp(nFlags, point);
}

void CEditWoodWnd::OnMouseMove(UINT nFlags, CPoint point)
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

void CEditWoodWnd::AdjustImageOrigin()
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

void CEditWoodWnd::LimitScalingFactor()
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

bool CEditWoodWnd::isCloseEnough(const CPoint& p1, const CPoint& p2, int threshold)
{
    //�Ȱѵ�һ����תΪ��Ļ��
    CPoint screenPoint = ImageToScreen(p1);
    int dx = screenPoint.x - p2.x;
    int dy = screenPoint.y - p2.y;
    return (dx * dx + dy * dy) <= (threshold * threshold);
}

bool CEditWoodWnd::isPointInEllipse(const CPoint& p)
{
    for (size_t i = 0; i < m_pScaleWood->wood_list.size(); i++)
    {
        if (p.x >= m_pScaleWood->wood_list[i].ellipse.cx - m_pScaleWood->wood_list[i].ellipse.ab1 * 0.5 &&
            p.x <= m_pScaleWood->wood_list[i].ellipse.cx + m_pScaleWood->wood_list[i].ellipse.ab1 * 0.5 &&
            p.y >= m_pScaleWood->wood_list[i].ellipse.cy - m_pScaleWood->wood_list[i].ellipse.ab2 * 0.5 &&
            p.y <= m_pScaleWood->wood_list[i].ellipse.cy + m_pScaleWood->wood_list[i].ellipse.ab2 * 0.5)
        {
            m_pScaleWood->wood_list[i].isDeleting = !m_pScaleWood->wood_list[i].isDeleting;
            return true;
        }
    }
    return false;
}

bool CEditWoodWnd::isPointInEllipse(int px, int py)
{
    for (size_t i = 0; i < m_pScaleWood->wood_list.size(); i++)
    {
        double angle = m_pScaleWood->wood_list[i].ellipse.angel;
        int cx = m_pScaleWood->wood_list[i].ellipse.cx;
        int cy = m_pScaleWood->wood_list[i].ellipse.cy;
        int a = m_pScaleWood->wood_list[i].ellipse.ab1;
        int b = m_pScaleWood->wood_list[i].ellipse.ab2;

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
            m_pScaleWood->wood_list[i].isDeleting = !m_pScaleWood->wood_list[i].isDeleting;
            return true;
        }
    }
    return false;
}

bool CEditWoodWnd::IsPointInEllipse(int px, int py, int cx, int cy, int a, int b, double angle)
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

bool CEditWoodWnd::IsPointInPolygon(int px, int py, const std::vector<CPoint>& polygon)
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

double CEditWoodWnd::CalculateAngle(int startX, int startY, int endX, int endY)
{
    //����Ҫ���
    return 0.0;
    // ��������֮��ĽǶȣ��Զ�Ϊ��λ��
    double dx = endX - startX;
    double dy = endY - startY;
    double angle = atan2(dy, dx) * 180.0 / M_PI;

    return angle;
}

void CEditWoodWnd::OnBnClickedBtnAdd()
{
    if (GetStatus() == -1 || GetScaleWoodID() <= 0)
    {
        AfxMessageBox(_T("����ʶ��"));
        return;
    }
    else if (GetStatus() == 0)
    {
        SetStatus(1);
        ResetBtnBgColor();
        m_btnAdd.SetBackgroundColor(RGB(241, 112, 122));
    }
    else if (GetStatus() == 1)
    {
        SetStatus(0);
        ResetBtnBgColor();
    }
    else
    {
        SetStatus(1);
        ResetBtnBgColor();
        m_btnAdd.SetBackgroundColor(RGB(241, 112, 122));
    }
}

void CEditWoodWnd::OnBnClickedBtnCrop()
{
    if (GetStatus() == -1 || GetScaleWoodID() <= 0)
    {
        AfxMessageBox(_T("����ʶ��"));
        return;
    }
    else if (GetStatus() == 3)
    {
        SetStatus(0);
        ResetBtnBgColor();
    }
    else
    {
        SetStatus(3);
        ResetBtnBgColor();
        m_btnCrop.SetBackgroundColor(RGB(241, 112, 122));
    }
}

void CEditWoodWnd::OnBnClickedBtnDel()
{
    if (GetStatus() == -1 || GetScaleWoodID() <= 0)
    {
        AfxMessageBox(_T("����ʶ��"));
        return;
    }
    else if (GetStatus() == 2)
    {
        SetStatus(0);
        ResetBtnBgColor();
    }
    else
    {
        SetStatus(2);
        ResetBtnBgColor();
        m_btnDel.SetBackgroundColor(RGB(241, 112, 122));
    }
}

void CEditWoodWnd::OnBnClickedBtnSave()
{
    m_pScaleWood->img = "save";
    ::PostMessage(GetParent()->GetSafeHwnd(), WM_CLOSE, NULL, NULL);
   
}

void CEditWoodWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
    //����Ƿ���ɾ�����İ���
    if (nChar == VK_DELETE && m_status == 2)
    {
        bool isDelete = false;
        auto iter = m_pScaleWood->wood_list.begin();
        while (iter != m_pScaleWood->wood_list.end())
        {
            if (iter->isDeleting)
            {
                iter = m_pScaleWood->wood_list.erase(iter);
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
            imagePath.Format(_T("%s%d_%d.jpg"), GetImagePath(), m_pScaleWood->id, m_wndIndex);
            LoadLocalImage(imagePath, false);
            Invalidate();
        }
    }
    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CEditWoodWnd::ResetCapture()
{
    m_image.Destroy();
    //m_btnAdd.ShowWindow(SW_SHOW);
    //m_btnDel.ShowWindow(SW_HIDE);
    //m_btnCrop.ShowWindow(SW_HIDE);

    m_points.clear();

    m_dragging = false;
    m_lastMousePos = CPoint();
    m_imageOrigin = CPoint();
    m_status = 0;
    m_scaleFactor = 1.0;
    m_startPoint = CPoint();
    m_endPoint = CPoint();

    m_isDrawFinished = false;
    //m_scaleWood = { 0 };
    m_ellipse_add = { 0 };

    this->Invalidate();
}


void CEditWoodWnd::ShowHistoryData(ScaleWood* pScaleWood)
{
    ResetCapture();

    m_pScaleWood->id = pScaleWood->id;
    m_pScaleWood->img = pScaleWood->img;
    m_pScaleWood->wood_list = pScaleWood->wood_list;

    CString imagePath;
    imagePath.Format(_T("%s%d_%d.jpg"), GetImagePath(), m_pScaleWood->id, m_wndIndex);
    //m_image.Load(imagePath); // ��"path_to_your_image"�滻Ϊ���ͼƬ·��
    LoadLocalImage(imagePath, true);
    SetStatus(0);
    ResetBtnBgColor();
    //::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
    //m_btnAdd.ShowWindow(SW_HIDE);
    //m_btnDel.ShowWindow(SW_HIDE);
    //m_btnCrop.ShowWindow(SW_HIDE);
    this->Invalidate();
}

CPoint CEditWoodWnd::ScreenToImage(CPoint screenPoint)
{
    // ����Ļ����ת��Ϊ���ڵĿͻ�������
    //ScreenToClient(&screenPoint);

    // �������ź�ͼƬ��ԭ��λ�ã����ͻ�������ת��ΪͼƬ����
    CPoint imagePoint;
    imagePoint.x = static_cast<int>((screenPoint.x - m_imageOrigin.x) / m_scaleFactor);
    imagePoint.y = static_cast<int>((screenPoint.y - m_imageOrigin.y) / m_scaleFactor);

    return imagePoint;
}

CPoint CEditWoodWnd::ImageToScreen(CPoint imagePoint)
{
    // ����Ļ����ת��Ϊ���ڵĿͻ�������
    //ScreenToClient(&screenPoint);

    // �������ź�ͼƬ��ԭ��λ�ã����ͻ�������ת��ΪͼƬ����
    CPoint screenPoint;
    screenPoint.x = static_cast<int>(imagePoint.x * m_scaleFactor + m_imageOrigin.x);
    screenPoint.y = static_cast<int>(imagePoint.y * m_scaleFactor + m_imageOrigin.y);

    return screenPoint;
}
//
//bool CEditWoodWnd::StartThread()
//{
//    //���������߳�
//    m_bRun = true;
//    m_recThread = AfxBeginThread(RecThread, (LPVOID)this);
//    if (m_recThread == NULL)
//    {
//        m_bRun = false;
//        return false;
//    }
//    m_hRecThreadHandle = m_recThread->m_hThread;
//    //m_evt_checkRecordEvent.SetEvent();
//    return true;
//}
//
//
//UINT CEditWoodWnd::RecThread(LPVOID lpParam)
//{
//    CEditWoodWnd* pDecode = (CEditWoodWnd*)lpParam;
//    if (!pDecode)
//    {
//        return 0;
//    }
//    while (WaitForSingleObject(pDecode->m_evt_beginRecEvent, INFINITE) == WAIT_OBJECT_0)
//    {
//        if (pDecode->m_bRun == false) break;
//        pDecode->SetRecing(true);
//        pDecode->RecMethod();
//        pDecode->SetRecing(false);
//        if (pDecode->m_bRun == false) break;
//    }
//
//    return 1;
//}
//
//bool CEditWoodWnd::StopThread()
//{
//    m_recing = false;
//    m_bRun = false;
//    m_evt_beginRecEvent.SetEvent();
//    if (m_hRecThreadHandle != INVALID_HANDLE_VALUE)
//    {
//        WaitAndTermThread(m_hRecThreadHandle, 10000);
//        m_hRecThreadHandle = INVALID_HANDLE_VALUE;
//        m_recThread = NULL;
//    }
//    return true;
//}
//
//void CEditWoodWnd::RecMethod()
//{
//#if (QGDebug == 1 && CloudAPI == 0) 
//    m_btnCrop.EnableWindow(TRUE);
//    m_btnCrop.SetWindowTextW(_T("ʶ��"));
//    m_btnCrop.ShowWindow(SW_HIDE);
//    m_btnDel.ShowWindow(SW_HIDE);
//    ScaleWood scalewood;
//    scalewood.id = time(0);
//    scalewood.img = "";
//    WoodAttr woodAttr1 = { 0 };
//    woodAttr1.ellipse = { 2436.7294921875000, 963.02606201171875, 54.000000000000000, 57.000000000000000, 63.426303863525391,
//        2383.0000000000000, 984.00000000000000, 2489.0000000000000, 943.00000000000000,
//        2455.0000000000000, 1013.0000000000000, 2417.0000000000000, 912.00000000000000 };
//    woodAttr1.diameter = 13.400000000000000;
//    woodAttr1.diameters = { 13.400000000000000, 14.300000000000001 };
//    woodAttr1.volumn = 0.0000000000000000;
//
//    WoodAttr woodAttr2 = { 0 };
//    woodAttr2.ellipse = { 2258.6398925781250, 1551.4863281250000, 56.000000000000000, 57.000000000000000, 124.72299957275391,
//        2231.0000000000000, 1502.0000000000000, 2285.0000000000000, 1602.0000000000000,
//        2210.0000000000000, 1578.0000000000000, 2307.0000000000000, 1527.0000000000000 };
//    woodAttr2.diameter = 5.0000000000000000;
//    woodAttr2.diameters = { 5.0000000000000000, 5.2000000000000002 };
//    woodAttr2.volumn = 0.0000000000000000;
//
//    WoodAttr woodAttr3 = { 0 };
//    woodAttr3.ellipse = { 2189.5292968750000, 1670.6750488281250, 73.000000000000000, 79.000000000000000, 120.99822998046875,
//        2120.0000000000000, 1635.0000000000000, 2260.0000000000000, 1706.0000000000000,
//        2158.0000000000000, 1735.0000000000000, 2222.0000000000000, 1606.0000000000000 };
//    woodAttr3.diameter = 6.5999999999999996;
//    woodAttr3.diameters = { 6.5999999999999996, 7.2000000000000002 };
//    woodAttr3.volumn = 0.0000000000000000;
//
//    WoodAttr woodAttr4 = { 0 };
//    woodAttr4.ellipse = { 2362.7504882812500, 1676.9362792968750, 65.000000000000000, 71.000000000000000, 52.447589874267578,
//        2306.0000000000000, 1720.0000000000000, 2417.0000000000000, 1632.0000000000000,
//        2403.0000000000000, 1726.0000000000000, 2320.0000000000000, 1625.0000000000000 };
//    woodAttr4.diameter = 6.0000000000000000;
//    woodAttr4.diameters = { 6.0000000000000000, 6.5000000000000000 };
//    woodAttr4.volumn = 0.0000000000000000;
//
//    scalewood.wood_list.push_back(woodAttr1);
//    scalewood.wood_list.push_back(woodAttr2);
//    scalewood.wood_list.push_back(woodAttr3);
//    scalewood.wood_list.push_back(woodAttr4);
//
//    std::string strImagePath = GetImagePathUTF8() + "img.jpg";
//    cv::Mat img = cv::imread(strImagePath);
//    strImagePath = GetImagePathUTF8() + std::to_string(scalewood.id) + ".jpg";
//    cv::imwrite(strImagePath, img);
//
//#else
//    CWaitCursor wait;
//    m_btnCrop.SetWindowTextW(_T("ʶ����"));
//    m_btnCrop.EnableWindow(FALSE);
//    m_btnDel.ShowWindow(SW_HIDE);
//    int errorCode = 0;
//    ScaleWood scalewood = { 0 };
//#if CloudAPI
//    int w = 0, h = 0, c = 0;
//    int ret = PostInfer(scalewood, errorCode, m_limg, m_rimg, m_camparam, w, h, c);
//#else
//    int ret = PostScale(scalewood, errorCode);
//#endif
//    //m_limg.clear();
//    //m_rimg.clear();
//    //m_camparam.clear();
//    scalewood.id = time(0);
//    m_btnCrop.EnableWindow(TRUE);
//    m_btnCrop.SetWindowTextW(_T("ʶ��"));
//    wait.Restore();
//    if (ret < 0)
//    {
//        m_btnCrop.ShowWindow(SW_SHOWNORMAL);
//        m_btnDel.ShowWindow(SW_SHOWNORMAL);
//        CString tipStr;
//        tipStr.Format(_T("ʶ��ʧ�ܣ�������, code:%d"), errorCode);
//        AfxMessageBox(tipStr);
//        return;
//    }
//    else
//    {
//        m_btnCrop.ShowWindow(SW_HIDE);
//    }
//    try
//    {
//        scalewood.img = base64_decode(scalewood.img);
//    }
//    catch (const std::exception&)
//    {
//        WriteLog(_T("invalid base64"));
//        m_btnCrop.ShowWindow(SW_SHOWNORMAL);
//        m_btnDel.ShowWindow(SW_SHOWNORMAL);
//        AfxMessageBox(_T("ʶ��ʧ�ܣ�������, invalid base64-1"));
//        return;
//    }
//    if (scalewood.img.length() <= 0)
//    {
//        m_btnCrop.ShowWindow(SW_SHOWNORMAL);
//        m_btnDel.ShowWindow(SW_SHOWNORMAL);
//        AfxMessageBox(_T("ʶ��ʧ�ܣ�������, invalid base64-2"));
//        return;
//    }
//#endif
//
//#if CloudAPI
//    cv::Mat img(w, h, CV_8UC3, (char*)scalewood.img.c_str());
//#else
//    /*std::vector<uchar> img_data(scalewood.img.begin(), scalewood.img.end());
//    cv::Mat img = cv::imdecode(cv::Mat(img_data), cv::IMREAD_COLOR);*/
//#endif
//    /*if (img.empty())
//    {
//        AfxMessageBox(_T("ʶ��ʧ�ܣ�������, invalid base64-3"));
//        return;
//    }
//    std::string imagePath = GetImagePathUTF8() + std::to_string(scalewood.id) + ".jpg";
//    cv::imwrite(imagePath, img);*/
//
//    m_image.Destroy();
//    CString strImagePathW;
//    strImagePathW.Format(_T("%s%d.jpg"), GetImagePath(), scalewood.id);
//    LoadLocalImage(strImagePathW, true);
//    m_scaleWood = scalewood;
//    SetStatus(0);
//    ::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
//    this->Invalidate();
//}