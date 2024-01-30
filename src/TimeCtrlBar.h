#pragma once
#include <afxwin.h>

class CTimeCtrlBar : public CWnd
{
public:
	CTimeCtrlBar() { }
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	double Progress() const;
	void SetProgress(double progress);

protected:
	double progress = 0.0;
	bool dragging = false;

	void PointToProgress(CPoint point);

	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP();
};
