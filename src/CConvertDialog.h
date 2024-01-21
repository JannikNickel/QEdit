#pragma once
#include "afxdialogex.h"

class CConvertDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CConvertDialog)

public:
	CConvertDialog(CWnd* pParent = nullptr);

protected:
	UINT_PTR timerId = 0;

	BOOL OnInitDialog() override;
	void UpdateProgress(float progress);

	afx_msg LRESULT OnConversionProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConversionCompleted(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
