#pragma once
#include <afxframewndex.h>
#include "MediaCtrlWnd.h"

class CMainFrame : public CFrameWndEx
{
	DECLARE_DYNCREATE(CMainFrame)

public:
	CMediaCtrlWnd* GetMediaCtrl();

protected:
	CMediaCtrlWnd mediaCtrlWnd;

	CMainFrame() noexcept { }

	void UpdateMediaCtrlLayout();
	void UpdateWindowVisibility();

	bool IsVideoLoaded();
	void ConvertCurrent();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnViewSizeChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMediaCtrlChanged();
	afx_msg void OnVideoLoaded();
	afx_msg void UpdateConvertButton(CCmdUI* pCmdUI);
	afx_msg void OnConvert();
	DECLARE_MESSAGE_MAP()
};
