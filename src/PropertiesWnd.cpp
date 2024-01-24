#include "pch.h"
#include "framework.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "QEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

CPropertiesWnd::CPropertiesWnd() noexcept
{

}

CPropertiesWnd::~CPropertiesWnd()
{

}

void CPropertiesWnd::AdjustLayout()
{
	if(GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}
	CRect rect;
	GetClientRect(rect);
	m_wndPropList.SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropertiesWnd::SetVSDotNetLook(BOOL bSet)
{
	m_wndPropList.SetVSDotNetLook(bSet);
	m_wndPropList.SetGroupNameFullWidth(bSet);
}

static int GetSelectedComboIndex(CMFCPropertyGridProperty* combo)
{
	const COleVariant& val = combo->GetValue();
	if(val.vt == VT_BSTR)
	{
		CString value = val.bstrVal;
		for(size_t i = 0; i < combo->GetOptionCount(); i++)
		{
			if(value == combo->GetOption(i))
			{
				return i;
			}
		}
	}
	return -1;
}

OutputSettings CPropertiesWnd::CurrentSettings() const
{
	VideoSettings video =
	{
		.resolution = { .width = vResWidthProp->GetValue().lVal, .height = vResHeightProp->GetValue().lVal },
		.fps = vFpsProp->GetValue().lVal,
		.bitrate = vBitrateProp->GetValue().lVal,
		.bitrateMode = static_cast<BitrateMode>(GetSelectedComboIndex(vBitrateModeProp)),
		.codec = static_cast<Codec>(GetSelectedComboIndex(vCodecProp))
	};
	AudioSettings audio =
	{
		.mute = static_cast<int8_t>(aMuteProp->GetValue().boolVal),
		.bitrate = aBitrateProp->GetValue().lVal
	};
	return OutputSettings { .video = video, .audio = audio };
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CRect rect;
	rect.SetRectEmpty();
	if(!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rect, this, 2))
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

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea(FALSE);
	m_wndPropList.SetVSDotNetLook(TRUE);
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Video"));
	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Resolution"), 0, TRUE);
	vResWidthProp = new CMFCRangedPropertyGridProperty(_T("Width"), (_variant_t)1920, _T(""));
	vResWidthProp->SetRange(128, 3840);
	pSize->AddSubItem(vResWidthProp);
	vResHeightProp = new CMFCRangedPropertyGridProperty(_T("Height"), (_variant_t)1080, _T(""));
	vResHeightProp->SetRange(128, 3840);
	pSize->AddSubItem(vResHeightProp);
	pGroup2->AddSubItem(pSize);
	pSize->Expand();
	vFpsProp = new CMFCRangedPropertyGridProperty(_T("Fps"), (_variant_t)60, _T(""));
	vFpsProp->SetRange(1, 300);
	pGroup2->AddSubItem(vFpsProp);
	vBitrateProp = new CMFCRangedPropertyGridProperty(_T("Bitrate"), (_variant_t)8000, _T(""));
	vBitrateProp->SetRange(128, 100000);
	pGroup2->AddSubItem(vBitrateProp);
	vBitrateModeProp = new CMFCPropertyGridProperty(_T("Bitrate Mode"), _T("Constant"), _T(""));
	vBitrateModeProp->AddOption(_T("Constant"));
	vBitrateModeProp->AddOption(_T("Variable"));
	vBitrateModeProp->AllowEdit(FALSE);
	pGroup2->AddSubItem(vBitrateModeProp);
	vCodecProp = new CMFCPropertyGridProperty(_T("Codec"), _T("H264"), _T(""));
	vCodecProp->AddOption(_T("H264"));
	vCodecProp->AddOption(_T("H265 (HEVC)"));
	vCodecProp->AddOption(_T("MPEG4"));
	vCodecProp->AllowEdit(FALSE);
	pGroup2->AddSubItem(vCodecProp);
	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Audio"));
	aMuteProp = new CMFCPropertyGridProperty(_T("Mute"), (_variant_t)false, _T(""));
	pGroup3->AddSubItem(aMuteProp);
	aBitrateProp = new CMFCRangedPropertyGridProperty(_T("Bitrate"), (_variant_t)128, _T(""));
	aBitrateProp->SetRange(32, 1024);
	pGroup3->AddSubItem(aBitrateProp);
	m_wndPropList.AddProperty(pGroup3);
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);
	afxGlobalData.GetNonClientMetrics(info);

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);
	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);
	m_wndPropList.SetFont(&m_fntPropList);
}
