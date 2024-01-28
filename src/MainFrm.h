#pragma once
#include "CMediaCtrlWnd.h"

class CMainFrame : public CFrameWndEx
{
protected:
	DECLARE_DYNCREATE(CMainFrame)
	CMainFrame() noexcept;

public:
	virtual ~CMainFrame();

	CMediaCtrlWnd* GetMediaCtrl();

protected:
	CMFCMenuBar m_wndMenuBar;
	CMediaCtrlWnd m_wndMediaCtrl;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnViewSizeChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMediaCtrlChanged();
	afx_msg void OnVideoLoaded();
	afx_msg void UpdateConvertButton(CCmdUI* pCmdUI);
	afx_msg void OnConvert();
	DECLARE_MESSAGE_MAP()

	void UpdateMediaCtrlLayout();
	void UpdateWindowVisibility();

	bool IsVideoLoaded();
	void ConvertCurrent();
};
