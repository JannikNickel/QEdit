#pragma once

/*
TODO
Add a start/end indicator which is connected above the actual toolbar to modify the start and end position by dragging the indicators
*/
class CTimeCtrlBar : public CWnd
{
public:
	CTimeCtrlBar() noexcept { }

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	float Progress() const;
	void SetProgress(float progress);

protected:
	float m_Progress = 0.0f;
	bool m_Dragging = false;

	void PointToProgress(CPoint point);

	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP();
};
