#pragma once
#include <afxdialogex.h>

class CConvertDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CConvertDialog)

public:
	CEvent dialogOpened;

	CConvertDialog(CWnd* pParent = nullptr);

protected:
	bool opened = false;

	void UpdateProgress(float progress);
	void OnOpen();

	afx_msg void OnClose();
	afx_msg void OnWindowPosChanged(WINDOWPOS* wndPos);
	afx_msg LRESULT OnConversionProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConversionCompleted(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
