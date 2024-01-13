#include "pch.h"
#include "framework.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "QEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar
CPropertiesWnd::CPropertiesWnd() noexcept
{

}

CPropertiesWnd::~CPropertiesWnd()
{

}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers
void CPropertiesWnd::AdjustLayout()
{
	if(GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if(!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;
	}

	InitPropList();
	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea(FALSE);
	m_wndPropList.SetVSDotNetLook(TRUE);
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Time"));
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Start"), (_variant_t)0.0f, _T("")));
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("End"), (_variant_t)0.0f, _T("")));
	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Video"));
	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Resolution"), 0, TRUE);
	CMFCPropertyGridProperty* pWidth = new CMFCPropertyGridProperty(_T("Width"), (_variant_t)1920, _T(""));
	pSize->AddSubItem(pWidth);
	CMFCPropertyGridProperty* pHeight = new CMFCPropertyGridProperty(_T("Height"), (_variant_t)1080, _T(""));
	pSize->AddSubItem(pHeight);
	pGroup2->AddSubItem(pSize);
	pSize->Expand();
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Fps"), (_variant_t)60, _T("")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Bitrate"), (_variant_t)8000, _T("")));
	CMFCPropertyGridProperty* pBitrateMode = new CMFCPropertyGridProperty(_T("Bitrate Mode"), _T("Constant"), _T(""));
	pBitrateMode->AddOption(_T("Constant"));
	pBitrateMode->AddOption(_T("Variable"));
	pBitrateMode->AllowEdit(FALSE);
	pGroup2->AddSubItem(pBitrateMode);
	CMFCPropertyGridProperty* pCodec = new CMFCPropertyGridProperty(_T("Codec"), _T("H264"), _T(""));
	pCodec->AddOption(_T("H264"));
	pCodec->AddOption(_T("H265 (HEVC)"));
	pCodec->AddOption(_T("MPEG4"));
	pCodec->AllowEdit(FALSE);
	pGroup2->AddSubItem(pCodec);
	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Audio"));
	pGroup3->AddSubItem(new CMFCPropertyGridProperty(_T("Mute"), (_variant_t)false, _T("")));
	pGroup3->AddSubItem(new CMFCPropertyGridProperty(_T("Bitrate"), (_variant_t)128, _T("")));
	m_wndPropList.AddProperty(pGroup3);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);
	m_wndPropList.SetFont(&m_fntPropList);
}
