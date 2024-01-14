#pragma once
#include <afxdlgs.h>
#include "CTimeCtrlBar.h"

class CMediaCtrlWnd : public CDialogEx
{
public:
	CMediaCtrlWnd(CWnd* pParent = nullptr);

	float CurrentTime() const;
	void SetVideoInfo(float totalTime, int totalFrames);

protected:
	CTimeCtrlBar m_wndTimeCtrl;
	float totalTime = 0.0f;
	int totalFrames = 0;

	BOOL OnInitDialog() override;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMediaCtrlChanged();

	DECLARE_MESSAGE_MAP();
};
