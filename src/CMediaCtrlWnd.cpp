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
END_MESSAGE_MAP()

CMediaCtrlWnd::CMediaCtrlWnd(CWnd* pParent) : CDialogEx(IDD_MEDIACTRL, pParent)
{
	
}

CMediaCtrlWnd::~CMediaCtrlWnd()
{
	delete timer;
}

float CMediaCtrlWnd::CurrentTime() const
{
	return m_wndTimeCtrl.Progress();
}

void CMediaCtrlWnd::SetVideoInfo(float totalTime, int totalFrames, float fps)
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

void CMediaCtrlWnd::OnHighResTimer(float dt)
{
	static float t = 0.0f;
	float targetStep = 1.0f / (videoFps != 0.0f ? videoFps : 1.0f);
	t += dt;
	if(t >= targetStep)
	{
		t = 0.0f;
		if(GetSafeHwnd())
		{
			SendMessage(WM_COMMAND, ID_CUSTOM_MEDIA_CTRL_PLAYBACK_THREAD_CHANGED);
		}
	}
}

void CMediaCtrlWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_wndTimeCtrl.GetSafeHwnd())
	{
		const int leftMargin = 100;
		const int rightMargin = 100;
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
	float p = m_wndTimeCtrl.Progress();

	CStatic* m_wndTimeLabel = (CStatic*)GetDlgItem(IDC_MCTRL_TEXT_TIME);
	if(m_wndTimeLabel != nullptr)
	{
		CString timeStr;
		timeStr.Format(_T("%.1f / %.1fs"), p * totalTime, totalTime);
		m_wndTimeLabel->SetWindowText(timeStr);
	}

	CStatic* m_wndFrameLabel = (CStatic*)GetDlgItem(IDC_MCTRL_TEXT_FRAME);
	if(m_wndFrameLabel != nullptr)
	{
		CString frameStr;
		frameStr.Format(_T("%d / %d"), static_cast<int>(roundf(p * totalFrames)), totalFrames);
		m_wndFrameLabel->SetWindowText(frameStr);
	}

	GetParent()->SendMessage(WM_COMMAND, ID_CUSTOM_MEDIA_CTRL_CHANGED);
}

void CMediaCtrlWnd::OnMediaCtrlPlaybackThreadChanged()
{
	if(isPlaying)
	{
		float p = m_wndTimeCtrl.Progress() + (1.0f / videoFps / totalTime);
		m_wndTimeCtrl.SetProgress(p);
		if(p >= 1.0f)
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
