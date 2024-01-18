#include "pch.h"
#include "CMediaCtrlWnd.h"
#include "resource.h"
#include "MainFrm.h"

#include "usermsg.h"

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
	return m_wndTimeCtrl.Progress();
}

void CMediaCtrlWnd::SetVideoInfo(double totalTime, int totalFrames, double fps)
{
	this->totalTime = totalTime;
	this->totalFrames = totalFrames;
	this->videoFps = fps;
}

BOOL CMediaCtrlWnd::OnInitDialog()
{
	if(!CDialogEx::OnInitDialog())
	{
		return FALSE;
	}
	if(!m_wndTimeCtrl.Create(DS_CONTROL | WS_VISIBLE, CRect(45, 10, 500, 15), this, IDC_MCTRL_TIMELINE_PROGRESS))
	{
		TRACE0("Failed to create time control window\n");
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
	double p = m_wndTimeCtrl.Progress() + (seconds / totalTime);
	m_wndTimeCtrl.SetProgress(p);
}

void CMediaCtrlWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_wndTimeCtrl.GetSafeHwnd())
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
		m_wndTimeCtrl.MoveWindow(rect);
	}
}

void CMediaCtrlWnd::OnMediaCtrlChanged()
{
	double p = m_wndTimeCtrl.Progress();

	CStatic* m_wndTimeLabel = (CStatic*)GetDlgItem(IDC_MCTRL_TEXT_TIME);
	if(m_wndTimeLabel != nullptr)
	{
		CString timeStr;
		timeStr.Format(_T("%.1f / %.1fs\n"), p * totalTime, totalTime);
		m_wndTimeLabel->SetWindowText(timeStr);
	}

	/*CStatic* m_wndFrameLabel = (CStatic*)GetDlgItem(IDC_MCTRL_TEXT_FRAME);
	if(m_wndFrameLabel != nullptr)
	{
		CString frameStr;
		frameStr.Format(_T("%d / %d"), static_cast<int>(roundf(p * totalFrames)), totalFrames);
		m_wndFrameLabel->SetWindowText(frameStr);
	}*/

	GetParent()->SendMessage(WM_COMMAND, ID_CUSTOM_MEDIA_CTRL_CHANGED);
}

void CMediaCtrlWnd::OnMediaCtrlPlaybackThreadChanged()
{
	if(isPlaying)
	{
		MoveProgress(1.0 / videoFps);
		if(m_wndTimeCtrl.Progress() >= 1.0)
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
