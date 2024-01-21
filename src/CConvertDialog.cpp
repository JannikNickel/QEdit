#include "pch.h"
#include "afxdialogex.h"
#include "CConvertDialog.h"
#include "resource.h"
#include "usermsg.h"
#include <algorithm>

IMPLEMENT_DYNAMIC(CConvertDialog, CDialogEx)

BEGIN_MESSAGE_MAP(CConvertDialog, CDialogEx)
	ON_MESSAGE(WM_CUSTOM_CONVERSION_PROGRESS, &CConvertDialog::OnConversionProgress)
	ON_MESSAGE(WM_CUSTOM_CONVERSION_COMPLETED, &CConvertDialog::OnConversionCompleted)
END_MESSAGE_MAP()

CConvertDialog::CConvertDialog(CWnd* pParent) : CDialogEx(IDD_CONVERTPROGRESS, pParent)
{
	
}

BOOL CConvertDialog::OnInitDialog()
{
	if(!CDialogEx::OnInitDialog())
	{
		return FALSE;
	}
	timerId = SetTimer(ID_TIMER_CONVERT_DLG, 100, NULL);
	return TRUE;
}

void CConvertDialog::UpdateProgress(float progress)
{
	if(CProgressCtrl* pCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_CONVERT_PROGRESS)))
	{
		progress = std::clamp(progress, 0.0f, 1.0f) * 100;
		CString header;
		header.Format(_T("Converting... (%.1f%%)"), progress);
		this->SetWindowText(header);
		pCtrl->SetPos(static_cast<int>(round(progress)));
		Invalidate();
	}
}

LRESULT CConvertDialog::OnConversionProgress(WPARAM wParam, LPARAM lParam)
{
	float progress = reinterpret_cast<float&>(wParam);
	UpdateProgress(progress);
	return 0;
}

LRESULT CConvertDialog::OnConversionCompleted(WPARAM wParam, LPARAM lParam)
{
	bool success = static_cast<bool>(wParam);
	EndDialog(success ? IDOK : IDABORT);
	return 0;
}
