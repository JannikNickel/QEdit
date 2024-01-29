#pragma once
#include "CMediaCtrlWnd.h"

class CQEditView : public CView
{
protected: // create from serialization only
	CQEditView() noexcept;
	DECLARE_DYNCREATE(CQEditView)

// Attributes
public:
	CQEditDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
protected:

// Implementation
public:
	virtual ~CQEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	CMediaCtrlWnd* GetMediaCtrl() const;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in QEditView.cpp
inline CQEditDoc* CQEditView::GetDocument() const
   { return reinterpret_cast<CQEditDoc*>(m_pDocument); }
#endif

