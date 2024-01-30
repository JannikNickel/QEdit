#pragma once
#include <afxdialogex.h>
#include "OutputSettings.h"
#include "VideoHandle.h"

class COutputOptionsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(COutputOptionsDialog)

public:
	COutputOptionsDialog(VideoHandle* vidHandle, CWnd* pParent = nullptr);

	OutputSettings Result() const;

protected:
	CButton resBtn;
	CButton fpsBtn;
	CButton bitrateBtn;
	CButton codecBtn;
	CButton audioMuteBtn;
	CButton audioBitrateBtn;

	CEdit resWidthEdit;
	CEdit resHeightEdit;
	CEdit fpsEdit;
	CEdit bitrateEdit;
	CComboBox bitrateModeCombo;
	CComboBox codecCombo;
	CEdit audioBitrateEdit;

	bool initUIValues = false;
	VideoHandle* vidHandle = nullptr;
	OutputSettings result = { 0 };

	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override;
	void SetVideoDataToUI();
	void UpdateControlState();
	void ClampValues();
	void ReadResult();

	afx_msg void CheckboxChanged(UINT nID);
	afx_msg void OnOK();
	DECLARE_MESSAGE_MAP()
};
