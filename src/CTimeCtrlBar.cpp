#include "pch.h"
#include "usermsg.h"
#include "CTimeCtrlBar.h"
#include <algorithm>

BEGIN_MESSAGE_MAP(CTimeCtrlBar, CProgressCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

BOOL CTimeCtrlBar::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(GetGlobalData()->RegisterWindowClass(_T("QEdit:CTimeCtrlBar")), _T(""), dwStyle, rect, pParentWnd, nID);
}

double CTimeCtrlBar::Progress() const
{
	return m_Progress;
}

void CTimeCtrlBar::SetProgress(double progress)
{
	m_Progress = std::clamp(progress, 0.0, 1.0);
	Invalidate();
	GetParent()->SendMessage(WM_COMMAND, ID_CUSTOM_MEDIA_CTRL_CHANGED);
}

void CTimeCtrlBar::PointToProgress(CPoint point)
{
	CRect rect;
	GetClientRect(&rect);
	double p = point.x / static_cast<double>(rect.Width());
	SetProgress(p);
}

void CTimeCtrlBar::OnPaint()
{
	CPaintDC pDC = CPaintDC(this);

	CRect rect;
	GetClientRect(rect);

	pDC.FillSolidRect(rect, RGB(200, 200, 200));

	rect.right = rect.left + static_cast<int>(rect.Width() * m_Progress);
	pDC.FillSolidRect(rect, RGB(0, 120, 215));
}

void CTimeCtrlBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	PointToProgress(point);
	m_Dragging = true;
	SetCapture();
}

void CTimeCtrlBar::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);

	if(m_Dragging)
	{
		PointToProgress(point);
	}
}

void CTimeCtrlBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);

	if(m_Dragging)
	{
		m_Dragging = false;
		ReleaseCapture();
	}
}
