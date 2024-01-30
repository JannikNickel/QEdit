#pragma once
#include "MediaCtrlWnd.h"

class CQEditView : public CView
{
	DECLARE_DYNCREATE(CQEditView)

public:
	CQEditDoc* GetDocument() const;

	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	void OnInitialUpdate() override;
	void OnDraw(CDC* pDC) override;

protected:
	CQEditView() noexcept { }

	CMediaCtrlWnd* GetMediaCtrl() const;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};

