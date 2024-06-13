#include "pch.h"
#include "MyWnd.h"
#include <iostream>
#include "CDlgDiameter.h"

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
END_MESSAGE_MAP()

CMyWnd::CMyWnd()
{
	m_bDragging = FALSE;
    m_bDbClick = FALSE;
    m_status = 0;
    m_isPolygonComplete = false;
	//m_image.Load(_T("..\\Doc\\test.jpg")); // 将"path_to_your_image"替换为你的图片路径
}

CMyWnd::~CMyWnd()
{
    
}
int CMyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void CMyWnd::OnPaint()
{
	CPaintDC dc(this);

	if (!m_image.IsNull())
	{
        CRect rect;
        GetClientRect(&rect);
        if (m_status == 0 || (m_status == 1 && m_isDrawFinished))
        {
            // 需要先绘制图形，再绘制背景图，才能正确显示
            // 获取与图像关联的设备上下文
            CDC* pDC = CDC::FromHandle(m_image.GetDC());
            if (pDC)
            {
                // 绘制多个不填充的椭圆曲线
                for (int i = 0; i < m_ellipseRects.size(); ++i)
                {
                    if (m_ellipseRects[i].rect.TopLeft() == m_ellipseRects[i].rect.BottomRight())
                    {
                        continue;
                    }
                    // 创建画笔&选择画笔
                    CPen pen(PS_SOLID, 5, RGB(255, 0, 0)); // 2为边框的宽度，可以根据需要调整
                    CPen* pOldPen = pDC->SelectObject(&pen);

                    // 创建一个空画刷&选择空画刷
                    CBrush brush;
                    brush.CreateStockObject(NULL_BRUSH);
                    CBrush* pOldBrush = pDC->SelectObject(&brush);

                    // 绘制不填充的椭圆曲线
                    pDC->Ellipse(m_ellipseRects[i].rect);

                    
                    // 获取椭圆的中心点和长轴的两个端点
                    CPoint center(m_ellipseRects[i].rect.CenterPoint());
                    //CPoint end1(center.x + (m_ellipseRects[i].rect.Width() / 2), center.y);
                    //CPoint end2(center.x - (m_ellipseRects[i].rect.Width() / 2), center.y);
                    //// 计算长轴的长度
                    //double distance = sqrt(pow(end1.x - end2.x, 2) + pow(end1.y - end2.y, 2));
                    if (m_ellipseRects[i].diameter > 0.0001)
                    {
                        // 将长轴的长度绘制在椭圆中心点附近
                        CString strDistance;
                        strDistance.Format(_T("%.2f"), m_ellipseRects[i].diameter);
                        // 设置文本背景模式为透明
                        pDC->SetBkMode(TRANSPARENT);
                        pDC->TextOut(center.x - 20, center.y - 10, strDistance);
                    }
                   

                    // 恢复原来的画笔和画刷
                    pDC->SelectObject(pOldPen);
                    pDC->SelectObject(pOldBrush);
                }

                // 设置透明背景模式
                pDC->SetBkMode(TRANSPARENT);
                // 创建画笔和画刷
                CPen pen(PS_SOLID, 5, RGB(255, 0, 0)); // 红色边框
                CBrush* pOldBrush = static_cast<CBrush*>(pDC->SelectStockObject(NULL_BRUSH)); // 空画刷
                CPen* pOldPen = pDC->SelectObject(&pen);

                // 绘制多边形
                if (m_points.size() > 1)
                {
                    for (size_t i = 0; i < m_points.size() - 1; i++)
                    {
                        pDC->MoveTo(m_points[i]);
                        pDC->LineTo(m_points[i + 1]);
                    }
                    //dc.Polyline(&m_points[0], static_cast<int>(m_points.size()));
                    if (m_isPolygonComplete)
                    {
                        pDC->MoveTo(m_points.front());
                        pDC->LineTo(m_points.back());
                    }
                }
                // 恢复原来的画笔和画刷
                pDC->SelectObject(pOldPen);
                pDC->SelectObject(pOldBrush);


                // 释放设备上下文
                m_image.ReleaseDC();
            }
            //椭圆绘制成功后，再画背景图片
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
            if (m_bDbClick)
            {
                m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
            }
            else
            {
                m_image.Draw(dc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_image.GetWidth(), m_image.GetHeight());
            }

            // 创建画笔&选择画笔
            CPen pen(PS_SOLID, 2, RGB(255, 255, 255)); // 2为边框的宽度，可以根据需要调整
            CPen* pOldPen = dc.SelectObject(&pen);

            // 创建一个空画刷&选择空画刷
            CBrush brush;
            brush.CreateStockObject(NULL_BRUSH);
            CBrush* pOldBrush = dc.SelectObject(&brush);

            // 计算椭圆的位置和大小&绘制椭圆
            CRect rectEllipse(startPoint, endPoint);
            dc.Ellipse(rectEllipse);

            // 恢复原来的画笔和画刷
            dc.SelectObject(pOldPen);
            dc.SelectObject(pOldBrush);
        }
        else if (m_status == 3)
        {
            if (m_bDbClick)
            {
                m_image.Draw(dc, m_ptOffset.x, m_ptOffset.y);
            }
            else
            {
                m_image.Draw(dc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_image.GetWidth(), m_image.GetHeight());
            }
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
                //dc.Polyline(&m_points[0], static_cast<int>(m_points.size()));
                if (m_isPolygonComplete)
                {
                    dc.MoveTo(m_points.front());
                    dc.LineTo(m_points.back());
                }
            }
            // 恢复原来的画笔和画刷
            dc.SelectObject(pOldPen);
            dc.SelectObject(pOldBrush);
        }
    }
	else
	{
	    CRect rect;
	    GetClientRect(rect);
	    dc.FillSolidRect(rect, RGB(42, 42, 43));   //控件背景色
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
            // 记录起始点
            m_isDrawFinished = false;
            startPoint = point;
            m_ellipseRects.push_back({ CRect(startPoint, startPoint),0.0 });
        }
        else if (m_status == 3)
        {
            if (!m_isPolygonComplete)
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
		if (m_ellipseRects[m_ellipseRects.size() - 1].rect.TopLeft() != m_ellipseRects[m_ellipseRects.size() - 1].rect.BottomRight())
		{
			//需要用户来输入短直径
			CDlgDiameter dlg;
			INT_PTR nResponse = dlg.DoModal();
			if (nResponse == IDOK)
			{
				float d = dlg.GetDiameter();
				if (d > 0.001)
				{
					m_ellipseRects[m_ellipseRects.size() - 1].diameter = d;
				}
				else
				{
					m_ellipseRects.erase(--m_ellipseRects.end());
				}
			}
			else
			{
				m_ellipseRects.erase(--m_ellipseRects.end());
			}
        }
        else
        {
            m_ellipseRects.erase(--m_ellipseRects.end());
        }
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
        // 如果鼠标左键按下，并且正在移动，更新结束点并重绘
        if ((nFlags & MK_LBUTTON) && (startPoint != point))
        {
            // 更新结束点
            endPoint = point;
            if (m_ellipseRects.size() > 0)
            {  
                //拖动之后要重新计算起始点
                if (m_bDbClick)
                {
                    CPoint newStartPoint(startPoint.x - m_ptOffset.x, startPoint.y - m_ptOffset.y);
                    CPoint newEndPoint(endPoint.x - m_ptOffset.x, endPoint.y - m_ptOffset.y);
                    m_ellipseRects[m_ellipseRects.size() - 1].rect.SetRect(newStartPoint, newEndPoint);
                }
                else
                {
                    RECT rect;
                    GetClientRect(&rect);
                    CPoint newStartPoint(startPoint.x * m_image.GetWidth() * 1.0 / (rect.right - rect.left), 
                        startPoint.y * m_image.GetHeight() * 1.0 / (rect.bottom - rect.top));
                    CPoint newEndPoint(endPoint.x * m_image.GetWidth() * 1.0 / (rect.right - rect.left),
                        endPoint.y * m_image.GetHeight() * 1.0 / (rect.bottom - rect.top));
                    m_ellipseRects[m_ellipseRects.size() - 1].rect.SetRect(newStartPoint, newEndPoint);
                }
            }
            Invalidate();
        }
    }
    else if (m_status == 3)
    {
        //鼠标按下第一个点后，鼠标移动先加入第二个点
        if (!m_isPolygonComplete && m_points.size() == 1)
        {
            m_points.push_back(point);
        }
        //随着鼠标移动，不断修改最后一个点
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
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

void CMyWnd::OnBnClickedBtnCapture()
{
    //SetStatus(0);
    ShowWindow(SW_SHOW);
    //AfxMessageBox(_T("拍照"));
    m_image.Load(_T("..\\Doc\\test.jpg")); // 将"path_to_your_image"替换为你的图片路径
    m_btnCapture.ShowWindow(SW_HIDE);
    m_btnDis.ShowWindow(SW_SHOW);
    m_btnRec.ShowWindow(SW_SHOW);
    this->Invalidate();
}

void CMyWnd::OnBnClickedBtnRec()
{
    CWaitCursor wait;
    //SetCursor(::LoadCursor(NULL, IDC_WAIT));
    m_btnRec.SetWindowTextW(_T("识别中"));
    m_btnRec.EnableWindow(FALSE);
    m_btnDis.ShowWindow(SW_HIDE);
    Sleep(2000);
    //SetCursor(::LoadCursor(NULL, IDC_ARROW));
    wait.Restore();
    m_btnRec.ShowWindow(SW_HIDE);
    m_btnRec.SetWindowTextW(_T("识别"));
    m_btnRec.EnableWindow(TRUE);

    //AfxMessageBox(_T("调用识别接口"));
    m_ellipseRects.push_back({ CRect(50, 50, 150, 100), 20.0});
    m_ellipseRects.push_back({ CRect(200, 50, 300, 100), 20.0 });
    m_ellipseRects.push_back({ CRect(100, 150, 200, 200), 20.0 });
    m_ellipseRects.push_back({ CRect(250, 150, 350, 200), 20.0 });
    m_ellipseRects.push_back({ CRect(150, 250, 450, 500), 20.0 });
    m_ellipseRects.push_back({ CRect(950, 750, 1450, 1500), 20.0 });
    m_ellipseRects.push_back({ CRect(1050, 850, 1650, 1900), 20.0 });
    SetStatus(0);

    this->Invalidate();
}

void CMyWnd::OnBnClickedBtnDis()
{
    //AfxMessageBox(_T("放弃")); 
    m_image.Destroy();
    m_btnCapture.ShowWindow(SW_SHOW);
    m_btnDis.ShowWindow(SW_HIDE);
    m_btnRec.ShowWindow(SW_HIDE);
    this->Invalidate();
}