#pragma once
#include <afxdlgs.h>
#include "CTimeCtrlBar.h"
#include "HighResolutionTimer.h"

class CMediaCtrlWnd : public CDialogEx
{
public:
	CMediaCtrlWnd(CWnd* pParent = nullptr);
	~CMediaCtrlWnd();

	float CurrentTime() const;
	void SetVideoInfo(float totalTime, int totalFrames, float fps);

protected:
	CTimeCtrlBar m_wndTimeCtrl;
	bool isPlaying = false;

	float totalTime = 0.0f;
	int totalFrames = 0;
	float videoFps = 60.0f;

	HighResolutionTimer* timer = nullptr;

	BOOL OnInitDialog() override;
	void OnHighResTimer(float dt);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMediaCtrlChanged();
	afx_msg void OnMediaCtrlPlaybackThreadChanged();
	afx_msg void OnPlayButtonClicked();

	DECLARE_MESSAGE_MAP();
};
