#pragma once
#include "OutputSettings.h"
#include "CMFCRangedPropertyGridProperty.h"

class CPropertiesWnd : public CDockablePane
{
public:
	CPropertiesWnd() noexcept;
	virtual ~CPropertiesWnd();

	void AdjustLayout() override;
	void SetVSDotNetLook(BOOL bSet);

	OutputSettings CurrentSettings() const;

protected:
	CFont m_fntPropList;
	CMFCPropertyGridCtrl m_wndPropList;

	CMFCRangedPropertyGridProperty* vResWidthProp = nullptr;
	CMFCRangedPropertyGridProperty* vResHeightProp = nullptr;
	CMFCRangedPropertyGridProperty* vFpsProp = nullptr;
	CMFCRangedPropertyGridProperty* vBitrateProp = nullptr;
	CMFCPropertyGridProperty* vBitrateModeProp = nullptr;
	CMFCPropertyGridProperty* vCodecProp = nullptr;
	CMFCPropertyGridProperty* aMuteProp = nullptr;
	CMFCRangedPropertyGridProperty* aBitrateProp = nullptr;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();
};

