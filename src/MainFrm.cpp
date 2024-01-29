#include "pch.h"
#include "framework.h"

#include "QEdit.h"
#include "MainFrm.h"
#include "QEditDoc.h"
#include "COutputOptionsDialog.h"
#include "CConvertDialog.h"
#include "ConversionJob.h"

#include "usermsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_CUSTOM_VIEW_SIZE_CHANGED, &CMainFrame::OnViewSizeChanged)
	ON_COMMAND(ID_CUSTOM_MEDIA_CTRL_CHANGED, &CMainFrame::OnMediaCtrlChanged)
	ON_COMMAND(ID_CUSTOM_VIDEO_LOADED, &CMainFrame::OnVideoLoaded)
	ON_UPDATE_COMMAND_UI(ID_CONVERT, &CMainFrame::UpdateConvertButton)
	ON_COMMAND(ID_CONVERT, &CMainFrame::OnConvert)
END_MESSAGE_MAP()

CMainFrame::CMainFrame() noexcept
{

}

CMainFrame::~CMainFrame()
{

}

CMediaCtrlWnd* CMainFrame::GetMediaCtrl()
{
	return &m_wndMediaCtrl;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CFrameWndEx::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if(!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;
	}
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if(!m_wndMediaCtrl.Create(IDD_MEDIACTRL, this))
	{
		TRACE0("Failed to create media control window\n");
		return -1;
	}
	m_wndMediaCtrl.ShowWindow(TRUE);
	UpdateMediaCtrlLayout();
	UpdateWindowVisibility();

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
	return 0;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);
	UpdateMediaCtrlLayout();
}

LRESULT CMainFrame::OnViewSizeChanged(WPARAM wParam, LPARAM lParam)
{
	UpdateMediaCtrlLayout();
	return 0;
}

void CMainFrame::OnMediaCtrlChanged()
{
	GetActiveView()->Invalidate();
}

void CMainFrame::OnVideoLoaded()
{
	UpdateWindowVisibility();
}

void CMainFrame::UpdateConvertButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsVideoLoaded());
}

void CMainFrame::OnConvert()
{
	ConvertCurrent();
}

void CMainFrame::UpdateMediaCtrlLayout()
{
	if(m_wndMediaCtrl.GetSafeHwnd())
	{
		const int mediaCtrlHeightDDU = 25;
		const int bottomMargin = 10;
		const int lrMargin = 10;

		CRect dialogRect = CRect(0, 0, 0, mediaCtrlHeightDDU);
		MapDialogRect(m_wndMediaCtrl.GetSafeHwnd(), &dialogRect);
		int heightInPx = dialogRect.Height();

		CView* view = GetActiveView();
		if(view != nullptr)
		{
			CRect rect;
			view->GetClientRect(&rect);
			rect.top = rect.bottom - heightInPx - bottomMargin;
			rect.bottom -= bottomMargin;
			rect.left += lrMargin;
			rect.right -= lrMargin;
			m_wndMediaCtrl.MoveWindow(&rect);
		}
	}
}

void CMainFrame::UpdateWindowVisibility()
{
	if(m_wndMediaCtrl.GetSafeHwnd())
	{
		m_wndMediaCtrl.ShowWindow(IsVideoLoaded());
	}
}

bool CMainFrame::IsVideoLoaded()
{
	CQEditDoc* doc = dynamic_cast<CQEditDoc*>(GetActiveDocument());
	return doc != nullptr && doc->HasVideo();
}

void CMainFrame::ConvertCurrent()
{
	if(m_wndMediaCtrl.GetSafeHwnd())
	{
		m_wndMediaCtrl.StopPlayback();
	}

	VideoHandle* vidHandle = static_cast<CQEditDoc*>(GetActiveDocument())->GetVideoHandle();
	COutputOptionsDialog optionsDlg = COutputOptionsDialog(vidHandle, this);
	if(optionsDlg.DoModal() == IDOK)
	{
		CString filePath;
		CString filter = _T("Video Files (*.mp4)|*.mp4||");
		CFileDialog fileDlg = CFileDialog(FALSE, _T(".mp4"), _T(""), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, this);
		if(fileDlg.DoModal() == IDOK)
		{
			filePath = fileDlg.GetPathName();

			CConvertDialog convertDlg;
			OutputSettings settings = optionsDlg.Result();
			std::tie(settings.start, settings.end) = m_wndMediaCtrl.TimeRangeSec();
			ConversionJob job = ConversionJob(vidHandle, filePath, settings, [&convertDlg, firstCallback = true](float progress, const TCHAR* error) mutable
			{
				if(firstCallback)
				{
					WaitForSingleObject(convertDlg.dialogOpened, INFINITE);
					firstCallback = false;
				}

				if(HWND hwnd = convertDlg.GetSafeHwnd())
				{
					if(error != nullptr)
					{
						AfxMessageBox(error, MB_ICONERROR);
						::SendMessage(hwnd, WM_CUSTOM_CONVERSION_COMPLETED, static_cast<WPARAM>(false), 0);
					}
					else if(progress >= 1.0f)
					{
						::SendMessage(hwnd, WM_CUSTOM_CONVERSION_COMPLETED, static_cast<WPARAM>(true), 0);
					}
					else
					{
						::SendMessage(hwnd, WM_CUSTOM_CONVERSION_PROGRESS, reinterpret_cast<WPARAM&>(progress), 0);
					}
				}
			});

			if(convertDlg.DoModal() == IDOK)
			{
				ShellExecute(NULL, _T("open"), _T("explorer"), _T("/select,\"") + filePath + _T("\""), NULL, SW_SHOWDEFAULT);
				MessageBeep(MB_OK);
			}
		}
	}
}
