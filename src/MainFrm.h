#pragma once
#include "PropertiesWnd.h"
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
	CPropertiesWnd m_wndProperties;
	CMediaCtrlWnd m_wndMediaCtrl;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg LRESULT OnViewSizeChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMediaCtrlChanged();
	afx_msg void OnVideoLoaded();
	afx_msg void UpdateConvertButton(CCmdUI* pCmdUI);
	afx_msg void OnConvert();
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

	void UpdateMediaCtrlLayout();
	void UpdateWindowVisibility();

	bool IsVideoLoaded();
	void ConvertCurrent();
};
