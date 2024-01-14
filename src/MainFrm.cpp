#include "pch.h"
#include "framework.h"

#include "QEdit.h"
#include "MainFrm.h"

#include "usermsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_PROPERTIES, &CMainFrame::OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, &CMainFrame::OnUpdateViewProperties)
	ON_MESSAGE(WM_CUSTOM_VIEW_SIZE_CHANGED, &CMainFrame::OnViewSizeChanged)
	ON_COMMAND(ID_CUSTOM_MEDIA_CTRL_CHANGED, &CMainFrame::OnMediaCtrlChanged)
END_MESSAGE_MAP()

CMainFrame::CMainFrame() noexcept
{

}

CMediaCtrlWnd* CMainFrame::GetMediaCtrl()
{
	return &m_wndMediaCtrl;
}

CMainFrame::~CMainFrame()
{

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
	CDockingManager::SetDockingMode(DT_SMART);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	if(!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}
	m_wndProperties.EnableDocking(CBRS_ALIGN_RIGHT);
	DockPane(&m_wndProperties);

	if(!m_wndMediaCtrl.Create(IDD_MEDIACTRL, this))
	{
		TRACE0("Failed to create media control window\n");
		return -1;
	}
	m_wndMediaCtrl.ShowWindow(TRUE);
	UpdateMediaCtrlLayout();

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));

	return 0;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);
	UpdateMediaCtrlLayout();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFrameWndEx::PreCreateWindow(cs);
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if(!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create properties window\n");
		return FALSE;
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hPropertiesBarIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	return CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
}

//Event Handler
void CMainFrame::OnViewProperties()
{
	BOOL vis = m_wndProperties.IsWindowVisible();
	m_wndProperties.ShowPane(!vis, FALSE, !vis);
}

void CMainFrame::OnUpdateViewProperties(CCmdUI* pCmdUI)
{
	BOOL vis = m_wndProperties.IsWindowVisible();
	pCmdUI->SetCheck(vis);
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
