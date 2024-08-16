#include "pch.h"
#include "MyWnd2.h"
#include <iostream>
#include "CDlgDiameter.h"
#include "base64.h"
#include "LogFile.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "common.h"

#ifndef M_PI
#define M_PI   3.141592653589793238462643383279502884
#endif

#define IDC_BTN_CAPTURE                 8000+1
#define IDC_BTN_REC                     8000+2
#define IDC_BTN_DIS                     8000+3

BEGIN_MESSAGE_MAP(CMyWnd2, CWnd)
    ON_WM_PAINT()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_BTN_CAPTURE, &CMyWnd2::OnBnClickedBtnCapture)
    ON_BN_CLICKED(IDC_BTN_REC, &CMyWnd2::OnBnClickedBtnRec)
    ON_BN_CLICKED(IDC_BTN_DIS, &CMyWnd2::OnBnClickedBtnDis)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CMyWnd2::CMyWnd2()
	: m_scaleFactor(1.0f)
	, m_imageOrigin(0, 0)
	, m_dragging(false)
	, m_status(0)
{
    m_scaleWood = { 0 };
    m_ellipse_add = { 0 };
}

CMyWnd2::~CMyWnd2()
{
}

int CMyWnd2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    m_btnCapture.Create(_T("拍照"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_CAPTURE);
    m_btnRec.Create(_T("识别"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_REC);
    m_btnDis.Create(_T("放弃"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(20, 20, 120, 50), this, IDC_BTN_DIS);

    CRect rect;
    GetClientRect(&rect);
    m_btnCapture.MoveWindow(rect.right - 120, rect.Height() * 0.5, 100, 50);
    m_btnRec.MoveWindow(rect.right - 120, rect.Height() * 0.5 - 100, 100, 50);
    m_btnDis.MoveWindow(rect.right - 120, rect.Height() * 0.5 + 100, 100, 50);
    return 0;
}
BOOL CMyWnd2::LoadLocalImage(LPCTSTR lpszPath, bool firstInit)
{
    HRESULT hr = m_image.Load(lpszPath);
    if (FAILED(hr))
    {
        AfxMessageBox(_T("Failed to load image"));
        return FALSE;
    }
    if (firstInit)
    {
        //加载完了之后，初始缩放到最小
        CRect clientRect;
        GetClientRect(&clientRect);
        float minScaleX = static_cast<float>(clientRect.Width()) / m_image.GetWidth();
        float minScaleY = static_cast<float>(clientRect.Height()) / m_image.GetHeight();
        m_scaleFactor = std::max(minScaleX, minScaleY);
    }

    Invalidate();
    return TRUE;
}

void CMyWnd2::DrawRotatedEllipse(Gdiplus::Graphics* graphics, WoodAttr& woodAttr)
{
    // 创建并设置变换矩阵
    Gdiplus::Matrix matrix;
    matrix.Translate(woodAttr.ellipse.cx, woodAttr.ellipse.cy);
    matrix.Rotate(woodAttr.ellipse.angel);

    // 保存图形状态并应用变换
    GraphicsState state = graphics->Save();
    graphics->SetTransform(&matrix);

    // 绘制椭圆
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

    // 恢复图形状态
    graphics->Restore(state);

    // 绘制文字（短半轴长度）
    CString text;
    text.Format(_T("%.2f"), woodAttr.diameter);
    Gdiplus::FontFamily fontFamily(L"Arial");
    Gdiplus::Font font(&fontFamily, 30, FontStyleRegular, UnitPixel);
    Gdiplus::PointF pointF(static_cast<float>(woodAttr.ellipse.cx - woodAttr.ellipse.ab1 * 0.5), static_cast<float>(woodAttr.ellipse.cy - woodAttr.ellipse.ab2 * 0.3));
    Gdiplus::SolidBrush solidBrush(Gdiplus::Color(255, 255, 0, 0)); // 红色画刷
    graphics->DrawString(text, -1, &font, pointF, &solidBrush);

    // 恢复图形状态，以确保绘制其他图形不受影响
   // Matrix matrix0;
    //graphics->SetTransform(&matrix0); // 重置变换矩阵
}

void CMyWnd2::DrawRotatedEllipse(Gdiplus::Graphics* graphics, Gdiplus::Pen& pen, int cx, int cy, int ab1, int ab2, double angle)
{
    // 创建并设置变换矩阵
    Gdiplus::Matrix matrix;
    matrix.Translate(cx, cy);
    matrix.Rotate(angle);

    // 保存图形状态并应用变换
    GraphicsState state = graphics->Save();
    graphics->SetTransform(&matrix);

    // 绘制椭圆
    graphics->DrawEllipse(&pen, -ab1, -ab2, 2 * ab1, 2 * ab2);

    // 恢复图形状态
    graphics->Restore(state);
}

void CMyWnd2::DrawPolygon(Gdiplus::Graphics* graphics, PointF points[], int numPoints)
{
    Pen red_pen(Gdiplus::Color(255, 255, 0, 0), 2);
    graphics->DrawPolygon(&red_pen, points, numPoints);
}

void CMyWnd2::OnPaint()
{
    CPaintDC dc(this);
    CRect clientRect;
    GetClientRect(&clientRect);

	if (m_image.IsNull())
	{
		dc.FillSolidRect(clientRect, RGB(42, 42, 43));   //控件背景色
		m_btnCapture.ShowWindow(SW_SHOW);
		m_btnDis.ShowWindow(SW_HIDE);
		m_btnRec.ShowWindow(SW_HIDE);
        return;
	}

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
    //m_image.Draw(dc, destRect);

    if (!m_image.IsNull())
    {
        if (m_status == 0 || (m_status == 1 && m_isDrawFinished) || m_status == 2)
        {
            // 获取GDI+图形对象
            Gdiplus::Graphics graphics(m_image.GetDC());

            for (size_t i = 0; i < m_scaleWood.wood_list.size(); i++)
            {
                DrawRotatedEllipse(&graphics, m_scaleWood.wood_list[i]);
            }
            // 释放GDI+图形对象
            m_image.ReleaseDC();

            //椭圆绘制成功后，再画背景图片
            //::SetStretchBltMode(dc, HALFTONE);
            //::SetBrushOrgEx(dc, 0, 0, NULL);
            m_image.Draw(dc, destRect);
        }
        else if (m_status == 1)
        {
            //::SetStretchBltMode(dc, HALFTONE);
            //::SetBrushOrgEx(dc, 0, 0, NULL);
            m_image.Draw(dc, destRect);
            // 获取GDI+图形对象
            Gdiplus::Graphics graphics(dc.GetSafeHdc());
            // 创建一个画笔
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
            m_image.Draw(dc, destRect);
            // 设置透明背景模式
            dc.SetBkMode(TRANSPARENT);
            // 创建画笔和画刷
            CPen pen(PS_SOLID, 2, RGB(255, 255, 255)); // 红色边框
            CBrush* pOldBrush = static_cast<CBrush*>(dc.SelectStockObject(NULL_BRUSH)); // 空画刷
            CPen* pOldPen = dc.SelectObject(&pen);

            // 绘制多边形
            if (m_points.size() > 1)
            {
                for (size_t i = 0; i < m_points.size() - 1; i++)
                {
                    dc.MoveTo(m_points[i]);
                    dc.LineTo(m_points[i + 1]);
                }
            }
            // 恢复原来的画笔和画刷
            dc.SelectObject(pOldPen);
            dc.SelectObject(pOldBrush);
        }
    }
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
    if (!m_image.IsNull())
    {
        if (m_status == 0)
        {
            m_dragging = true;
            m_lastMousePos = point;
            SetCapture(); // 捕获鼠标，处理拖动事件
        }
        else if (m_status == 1)
        {
            // 记录起始点
            m_isDrawFinished = false;
            m_startPoint = point;
        }
        else if (m_status == 2)
        {
            m_dragging = true;
            m_lastMousePos = point;
			SetCapture(); // 捕获鼠标，处理拖动事件

			//判断当前点是否在某个椭圆内部
			CPoint curPoint = ScreenToImage(point);
			if (isPointInEllipse(curPoint.x, curPoint.y))
			{
				SetFocus();
				Invalidate();
			}
        }
        else if (m_status == 3)
        {
            // 左键点击时，记录顶点
            if (m_points.size() > 0 && isCloseEnough(m_points.front(), point, 10))
            {
                // 如果最后一个点与第一个点距离小于5，则完成多边形
                m_points[m_points.size() - 1].SetPoint(m_points.front().x, m_points.front().y);
                int ret = AfxMessageBox(_T("确认保留多边形区域的木材？"), MB_OKCANCEL);
                if (ret != IDOK)
                {
                    m_points.clear();
                }
				else
				{
					for (size_t i = 0; i < m_points.size(); i++)
					{
						m_points[i] = ScreenToImage(m_points[i]);
					}

					//处于多边形之外的数据直接剔除
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
					//之前的绘制的椭圆无效了，需要重新绘制
					m_image.Destroy();
					CString imagePath;
					imagePath.Format(_T("%s%d.jpg"), GetImagePath(), m_scaleWood.id);
					LoadLocalImage(imagePath, false);
					SetStatus(0);
					::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
					m_points.clear();
				}
			}
            else
            {
                m_points.push_back(point);
            }
            Invalidate();
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void CMyWnd2::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_status == 0 || m_status == 2)
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
            //需要用户来输入短直径
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

void CMyWnd2::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_status == 0 || m_status == 2)
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
    }
    else if (m_status == 1)
    {
        // 如果鼠标左键按下，并且正在移动，更新结束点并重绘
        if ((nFlags & MK_LBUTTON) && (m_startPoint != point))
        {
			// 更新结束点
			m_endPoint = point;
			//拖动之后要重新计算起始点
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
    else if (m_status == 3)
    {
        //鼠标按下第一个点后，鼠标移动先加入第二个点
        if (m_points.size() == 1)
        {
            m_points.push_back(point);
        }
        //随着鼠标移动，不断修改最后一个点
        if (m_points.size() > 1)
        {
            m_points[m_points.size() - 1].SetPoint(point.x, point.y);
        }

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
    float minScale = std::max(minScaleX, minScaleY);

    m_scaleFactor = std::max(m_scaleFactor, minScale);
    m_scaleFactor = std::min(m_scaleFactor, 10.0f); // 也可以限制最大缩放比例
}

bool CMyWnd2::isCloseEnough(const CPoint& p1, const CPoint& p2, int threshold)
{
    int dx = p1.x - p2.x;
    int dy = p1.y - p2.y;
    return (dx * dx + dy * dy) <= (threshold * threshold);
}

bool CMyWnd2::isPointInEllipse(const CPoint& p)
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

bool CMyWnd2::isPointInEllipse(int px, int py)
{
    for (size_t i = 0; i < m_scaleWood.wood_list.size(); i++)
    {
        double angle = m_scaleWood.wood_list[i].ellipse.angel;
        int cx = m_scaleWood.wood_list[i].ellipse.cx;
        int cy = m_scaleWood.wood_list[i].ellipse.cy;
        int a = m_scaleWood.wood_list[i].ellipse.ab1;
        int b = m_scaleWood.wood_list[i].ellipse.ab2;

        // 将角度转换为弧度
        double radianAngle = angle * M_PI / 180.0;

        // 将点(px, py)绕椭圆中心(cx, cy)反向旋转angle角度
        double cosAngle = cos(-radianAngle);
        double sinAngle = sin(-radianAngle);

        double dx = px - cx;
        double dy = py - cy;

        double rotatedX = dx * cosAngle - dy * sinAngle;
        double rotatedY = dx * sinAngle + dy * cosAngle;

        // 判断旋转后的点是否在标准椭圆内
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

bool CMyWnd2::IsPointInEllipse(int px, int py, int cx, int cy, int a, int b, double angle)
{
    // 将角度转换为弧度
    double radianAngle = angle * M_PI / 180.0;

    // 将点(px, py)绕椭圆中心(cx, cy)反向旋转angle角度
    double cosAngle = cos(-radianAngle);
    double sinAngle = sin(-radianAngle);

    double dx = px - cx;
    double dy = py - cy;

    double rotatedX = dx * cosAngle - dy * sinAngle;
    double rotatedY = dx * sinAngle + dy * cosAngle;

    // 判断旋转后的点是否在标准椭圆内
    double normalizedX = rotatedX / a;
    double normalizedY = rotatedY / b;

    return (normalizedX * normalizedX + normalizedY * normalizedY) <= 1.0;
}

bool CMyWnd2::IsPointInPolygon(int px, int py, const std::vector<CPoint>& polygon)
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

double CMyWnd2::CalculateAngle(int startX, int startY, int endX, int endY)
{
    //不需要倾角
    return 0.0;
    // 计算两点之间的角度（以度为单位）
    double dx = endX - startX;
    double dy = endY - startY;
    double angle = atan2(dy, dx) * 180.0 / M_PI;

    return angle;
}

void CMyWnd2::OnBnClickedBtnCapture()
{
#if (QGDebug == 1)
    ShowWindow(SW_SHOW);
#else
    ShowWindow(SW_SHOW);
    std::string limg;
    CWaitCursor wait;
    int errorCode = 0;
    int ret = PostPhoto(limg, errorCode);
    wait.Restore();
    if (ret < 0)
    {
        WriteLog(_T("PostPhoto API failed, errorCode:%d"), errorCode);
        CString tipStr;
        tipStr.Format(_T("拍照失败，请重试, code:%d"), errorCode);
        AfxMessageBox(tipStr);
        return;
    }
    try
    {
        limg = base64_decode(limg);
    }
    catch (const std::exception&)
    {
        WriteLog(_T("invalid base64 exception"));
        AfxMessageBox(_T("获取图片失败，请重试"));
        return;
    }
    if (limg.length() <= 0)
    {
        WriteLog(_T("invalid base64 limg.length() <= 0"));
        AfxMessageBox(_T("获取图片失败，请重试"));
        return;
    }

    std::vector<uchar> img_data(limg.begin(), limg.end());
    cv::Mat img = cv::imdecode(cv::Mat(img_data), cv::IMREAD_COLOR);
    cv::imwrite(GetImagePathUTF8() + "limg.jpg", img);
#endif
    LoadLocalImage(GetImagePath() + _T("limg.jpg"), true);
    m_btnCapture.ShowWindow(SW_HIDE);
    m_btnDis.ShowWindow(SW_SHOW);
    m_btnRec.ShowWindow(SW_SHOW);
    this->Invalidate();
}

void CMyWnd2::OnBnClickedBtnRec()
{
#if (QGDebug == 1) 
    m_btnRec.EnableWindow(TRUE);
    m_btnRec.SetWindowTextW(_T("识别"));
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

    std::string strImagePath = GetImagePathUTF8() + "img.jpg";
    cv::Mat img = cv::imread(strImagePath);
    strImagePath = GetImagePathUTF8() + std::to_string(scalewood.id) + ".jpg";
    cv::imwrite(strImagePath, img);

#else
    CWaitCursor wait;
    m_btnRec.SetWindowTextW(_T("识别中"));
    m_btnRec.EnableWindow(FALSE);
    m_btnDis.ShowWindow(SW_HIDE);
    int errorCode = 0;
    ScaleWood scalewood = { 0 };
    int ret = PostScale(scalewood, errorCode);
    scalewood.id = time(0);
    m_btnRec.EnableWindow(TRUE);
    m_btnRec.SetWindowTextW(_T("识别"));
    wait.Restore();
    if (ret < 0)
    {
        m_btnRec.ShowWindow(SW_SHOWNORMAL);
        m_btnDis.ShowWindow(SW_SHOWNORMAL);
        CString tipStr;
        tipStr.Format(_T("识别失败，请重试, code:%d"), errorCode);
        AfxMessageBox(tipStr);
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
        AfxMessageBox(_T("识别失败，请重试, invalid base64-1"));
        return;
    }
    if (scalewood.img.length() <= 0)
    {
        m_btnRec.ShowWindow(SW_SHOWNORMAL);
        m_btnDis.ShowWindow(SW_SHOWNORMAL);
        AfxMessageBox(_T("识别失败，请重试, invalid base64-2"));
        return;
    }
    //img,base64解密
    std::vector<uchar> img_data(scalewood.img.begin(), scalewood.img.end());
    cv::Mat img = cv::imdecode(cv::Mat(img_data), cv::IMREAD_COLOR);
    std::string imagePath = GetImagePathUTF8() + std::to_string(scalewood.id) + ".jpg";
    cv::imwrite(imagePath, img);
#endif

    m_image.Destroy();
    CString strImagePathW;
    strImagePathW.Format(_T("%s%d.jpg"), GetImagePath(), scalewood.id);
    LoadLocalImage(strImagePathW, true);
    m_scaleWood = scalewood;
    SetStatus(0);
    ::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
    this->Invalidate();
}

void CMyWnd2::OnBnClickedBtnDis()
{
    m_image.Destroy();
    m_btnCapture.ShowWindow(SW_SHOW);
    m_btnDis.ShowWindow(SW_HIDE);
    m_btnRec.ShowWindow(SW_HIDE);
    this->Invalidate();
}

void CMyWnd2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //检查是否是删除键的按下
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
            imagePath.Format(_T("%s%d.jpg"), GetImagePath(), m_scaleWood.id);
            LoadLocalImage(imagePath, false);
            Invalidate();
        }
    }
    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CMyWnd2::ResetCapture()
{
    m_image.Destroy();
    m_btnCapture.ShowWindow(SW_SHOW);
    m_btnDis.ShowWindow(SW_HIDE);
    m_btnRec.ShowWindow(SW_HIDE);

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

    this->Invalidate();
}


void CMyWnd2::ShowHistoryData(ScaleWood* pScaleWood)
{
    ResetCapture();

    m_scaleWood.id = pScaleWood->id;
    m_scaleWood.img = pScaleWood->img;
    m_scaleWood.wood_list = pScaleWood->wood_list;

    CString imagePath;
    imagePath.Format(_T("%s%d.jpg"), GetImagePath(), m_scaleWood.id);
    //m_image.Load(imagePath); // 将"path_to_your_image"替换为你的图片路径
    LoadLocalImage(imagePath, true);
    SetStatus(0);
    ::PostMessage(GetParent()->m_hWnd, WM_USER_MESSAGE_FINISHED, NULL, NULL);
    m_btnCapture.ShowWindow(SW_HIDE);
    m_btnDis.ShowWindow(SW_HIDE);
    m_btnRec.ShowWindow(SW_HIDE);
    this->Invalidate();
}

CPoint CMyWnd2::ScreenToImage(CPoint screenPoint)
{
    // 将屏幕坐标转换为窗口的客户区坐标
    //ScreenToClient(&screenPoint);

    // 考虑缩放和图片的原点位置，将客户区坐标转换为图片坐标
    CPoint imagePoint;
    imagePoint.x = static_cast<int>((screenPoint.x - m_imageOrigin.x) / m_scaleFactor);
    imagePoint.y = static_cast<int>((screenPoint.y - m_imageOrigin.y) / m_scaleFactor);

    return imagePoint;
}