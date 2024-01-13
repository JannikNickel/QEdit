#pragma once
#include "PropertiesWnd.h"

class CMainFrame : public CFrameWndEx
{
protected:
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

	// Attributes
public:

	// Operations
public:

	// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

	// Implementation
public:
	virtual ~CMainFrame();
	#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CPropertiesWnd    m_wndProperties;

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
};
