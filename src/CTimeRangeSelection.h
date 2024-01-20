#pragma once
#include <tuple>

class CTimeRangeSelection : public CWnd
{
public:
	CTimeRangeSelection() noexcept { }

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	std::tuple<double, double> Range() const;
	void SetRange(double from, double to);

protected:
	double from = 0.0;
	double to = 1.0;
	double* draggedValue = nullptr;

	DWORD color = RGB(140, 215, 0);
	int handleWidth = 6;

	double PointToProgress(CPoint point);
	void DrawHandle(CPaintDC& pDC, double pos, bool min);

	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP();
};
