#include "pch.h"
#include "usermsg.h"
#include "TimeRangeSelection.h"
#include <algorithm>

BEGIN_MESSAGE_MAP(CTimeRangeSelection, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

BOOL CTimeRangeSelection::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(GetGlobalData()->RegisterWindowClass(_T("QEdit:CTimeRangeSelection")), _T(""), dwStyle, rect, pParentWnd, nID);
}

std::tuple<double, double> CTimeRangeSelection::Range() const
{
	return std::make_tuple(from, to);
}

void CTimeRangeSelection::SetRange(double from, double to)
{
	from = min(from, to);
	to = max(from, to);
	this->from = std::clamp(from, 0.0, 1.0);
	this->to = std::clamp(to, 0.0, 1.0);
	Invalidate();
}

double CTimeRangeSelection::PointToProgress(CPoint point)
{
	CRect rect;
	GetClientRect(&rect);
	return point.x / static_cast<double>(rect.Width());
}

void CTimeRangeSelection::DrawHandle(CPaintDC& pDC, double pos, bool min)
{
	CRect rect;
	GetClientRect(&rect);

	int p = static_cast<int>(round(pos * rect.Width()));
	rect.left = p - handleWidth / 2;
	rect.right = p + handleWidth / 2;
	pDC.FillSolidRect(rect, color);
}

void CTimeRangeSelection::OnPaint()
{
	CPaintDC pDC = CPaintDC(this);

	CRect rect;
	GetClientRect(&rect);

	int width = rect.Width();
	rect.top += rect.Height() / 2;
	rect.left += static_cast<int>(width * from);
	rect.right -= static_cast<int>(width * (1.0 - to));

	pDC.FillSolidRect(rect, color);

	DrawHandle(pDC, from, true);
	DrawHandle(pDC, to, false);
}

void CTimeRangeSelection::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	double normPos = PointToProgress(point);
	draggedValue = abs(normPos - from) < abs(normPos - to) ? &from : &to;
	if(from == to)
	{
		draggedValue = normPos > from ? &to : &from;
	}
	SetCapture();
}

void CTimeRangeSelection::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);

	if(draggedValue != nullptr)
	{
		double p = PointToProgress(point);
		double min = draggedValue == &to ? from : 0.0;
		double max = draggedValue == &from ? to : 1.0;
		*draggedValue = std::clamp(p, min, max);
		Invalidate();
	}
}

void CTimeRangeSelection::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);

	if(draggedValue != nullptr)
	{
		draggedValue = nullptr;
		ReleaseCapture();
	}
}
