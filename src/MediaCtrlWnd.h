#pragma once
#include <afxdlgs.h>
#include <tuple>
#include "TimeCtrlBar.h"
#include "TimeRangeSelection.h"
#include "HighResolutionTimer.h"

class CMediaCtrlWnd : public CDialogEx
{
public:
	CMediaCtrlWnd(CWnd* pParent = nullptr);
	~CMediaCtrlWnd();

	double CurrentTime() const;
	std::tuple<double, double> TimeRangeSec();
	void InitVideoInfo(double totalTime, int totalFrames, double fps);
	void Reset();
	void StopPlayback();

protected:
	CTimeCtrlBar timeCtrlWnd;
	CTimeRangeSelection timeRangeSelectionWnd;
	bool isPlaying = false;

	double totalTime = 0.0;
	int totalFrames = 0;
	double videoFps = 60.0;

	HighResolutionTimer* timer = nullptr;

	BOOL OnInitDialog() override;
	void OnHighResTimer(double dt);
	void MoveProgress(double seconds);
	void UpdateTimeText();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMediaCtrlChanged();
	afx_msg void OnMediaCtrlPlaybackThreadChanged();
	afx_msg void OnPlayButtonClicked();
	afx_msg void OnPrevButtonClicked();
	afx_msg void OnNextButtonClicked();
	DECLARE_MESSAGE_MAP();
};
