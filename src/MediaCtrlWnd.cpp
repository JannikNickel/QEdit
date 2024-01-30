#include "pch.h"
#include "resource.h"
#include "usermsg.h"
#include "MediaCtrlWnd.h"
#include "MainFrm.h"

BEGIN_MESSAGE_MAP(CMediaCtrlWnd, CDialogEx)
	ON_WM_SIZE()
	ON_COMMAND(ID_CUSTOM_MEDIA_CTRL_CHANGED, &CMediaCtrlWnd::OnMediaCtrlChanged)
	ON_COMMAND(ID_CUSTOM_MEDIA_CTRL_PLAYBACK_THREAD_CHANGED, &CMediaCtrlWnd::OnMediaCtrlPlaybackThreadChanged)
	ON_BN_CLICKED(IDC_MCTRL_BTN_PLAY, &CMediaCtrlWnd::OnPlayButtonClicked)
	ON_BN_CLICKED(IDC_MCTRL_BTN_PREV, &CMediaCtrlWnd::OnPrevButtonClicked)
	ON_BN_CLICKED(IDC_MCTRL_BTN_NEXT, &CMediaCtrlWnd::OnNextButtonClicked)
END_MESSAGE_MAP()

CMediaCtrlWnd::CMediaCtrlWnd(CWnd* pParent) : CDialogEx(IDD_MEDIACTRL, pParent)
{

}

CMediaCtrlWnd::~CMediaCtrlWnd()
{
	delete timer;
}

double CMediaCtrlWnd::CurrentTime() const
{
	return timeCtrlWnd.Progress();
}

std::tuple<double, double> CMediaCtrlWnd::TimeRangeSec()
{
	auto [from, to] = timeRangeSelectionWnd.Range();
	from *= totalTime;
	to *= totalTime;
	return std::make_tuple(from, to);
}

void CMediaCtrlWnd::InitVideoInfo(double totalTime, int totalFrames, double fps)
{
	this->totalTime = totalTime;
	this->totalFrames = totalFrames;
	this->videoFps = fps;
	Reset();
}

void CMediaCtrlWnd::Reset()
{
	if(timeCtrlWnd.GetSafeHwnd())
	{
		timeCtrlWnd.SetProgress(0.0);
	}
	if(timeRangeSelectionWnd.GetSafeHwnd())
	{
		timeRangeSelectionWnd.SetRange(0.0, 1.0);
	}
	Invalidate();
}

void CMediaCtrlWnd::StopPlayback()
{
	if(isPlaying)
	{
		OnPlayButtonClicked();
	}
}

BOOL CMediaCtrlWnd::OnInitDialog()
{
	if(!CDialogEx::OnInitDialog())
	{
		return FALSE;
	}
	if(!timeCtrlWnd.Create(DS_CONTROL | WS_VISIBLE, CRect(45, 10, 500, 15), this, IDC_MCTRL_TIMELINE_PROGRESS))
	{
		TRACE0("Failed to create time control window\n");
		return FALSE;
	}
	if(!timeRangeSelectionWnd.Create(DS_CONTROL | WS_VISIBLE, CRect(45, 5, 500, 10), this, IDC_MCTRL_RANGE_SELECTION))
	{
		TRACE0("Failed to range selection window\n");
		return FALSE;
	}

	timer = new HighResolutionTimer(std::bind(&CMediaCtrlWnd::OnHighResTimer, this, std::placeholders::_1));
	return TRUE;
}

void CMediaCtrlWnd::OnHighResTimer(double dt)
{
	static double t = 0.0;
	double targetStep = 1.0 / (videoFps != 0.0 ? videoFps : 1.0);
	t += dt;
	if(t >= targetStep)
	{
		t = 0.0;
		if(GetSafeHwnd())
		{
			SendMessage(WM_COMMAND, ID_CUSTOM_MEDIA_CTRL_PLAYBACK_THREAD_CHANGED);
		}
	}
}

void CMediaCtrlWnd::MoveProgress(double seconds)
{
	double p = timeCtrlWnd.Progress() + (seconds / totalTime);
	timeCtrlWnd.SetProgress(p);
}

void CMediaCtrlWnd::UpdateTimeText()
{
	double p = timeCtrlWnd.Progress();

	CStatic* m_wndTimeLabel = (CStatic*)GetDlgItem(IDC_MCTRL_TEXT_TIME);
	if(m_wndTimeLabel != nullptr)
	{
		CString timeStr;
		timeStr.Format(_T("%.1f / %.1fs\n"), p * totalTime, totalTime);
		m_wndTimeLabel->SetWindowText(timeStr);
	}
}

void CMediaCtrlWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(timeCtrlWnd.GetSafeHwnd())
	{
		const int leftMargin = 110;
		const int rightMargin = 110;
		const int height = 10;

		CRect rect;
		GetClientRect(&rect);
		int rectHeight = rect.Height();
		rect.left += leftMargin;
		rect.right -= rightMargin;
		rect.top += (rectHeight - height) / 2;
		rect.bottom -= (rectHeight - height) / 2;
		timeCtrlWnd.MoveWindow(rect);

		rect.top = 5;
		rect.bottom = rect.top + 10;
		timeRangeSelectionWnd.MoveWindow(rect);
	}
}

void CMediaCtrlWnd::OnMediaCtrlChanged()
{
	UpdateTimeText();
	GetParent()->SendMessage(WM_COMMAND, ID_CUSTOM_MEDIA_CTRL_CHANGED);
}

void CMediaCtrlWnd::OnMediaCtrlPlaybackThreadChanged()
{
	if(isPlaying)
	{
		MoveProgress(1.0 / videoFps);
		if(timeCtrlWnd.Progress() >= 1.0)
		{
			OnPlayButtonClicked();
		}
	}
}

void CMediaCtrlWnd::OnPlayButtonClicked()
{
	CButton* button = (CButton*)GetDlgItem(IDC_MCTRL_BTN_PLAY);
	if(button != nullptr)
	{
		isPlaying = !isPlaying;
		button->SetWindowText(isPlaying ? _T("||") : _T(">"));
	}
}

void CMediaCtrlWnd::OnPrevButtonClicked()
{
	MoveProgress(-1.0 / (videoFps));
}

void CMediaCtrlWnd::OnNextButtonClicked()
{
	MoveProgress(1.0 / (videoFps));
}
