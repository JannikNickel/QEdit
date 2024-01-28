#pragma once
#include "afxdialogex.h"
#include "OutputSettings.h"
#include "VideoHandle.h"

class COutputOptionsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(COutputOptionsDialog)

public:
	COutputOptionsDialog(VideoHandle* vidHandle, CWnd* pParent = nullptr);

	OutputSettings Result() const;

protected:
	CButton m_ResCheck;
	CButton m_FpsCheck;
	CButton m_BitrateCheck;
	CButton m_CodecCheck;
	CButton m_AudioMuteCheck;
	CButton m_AudioBitrateCheck;

	CEdit m_ResWidthEdit;
	CEdit m_ResHeightEdit;
	CEdit m_FpsEdit;
	CEdit m_BitrateEdit;
	CComboBox m_BitrateModeCombo;
	CComboBox m_CodecCombo;
	CEdit m_AudioBitrateEdit;

	bool initUIValues = false;
	VideoHandle* vidHandle = nullptr;
	OutputSettings m_Result = { 0 };

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
