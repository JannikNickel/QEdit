#include "pch.h"
#include "afxdialogex.h"
#include "CConvertDialog.h"
#include "resource.h"
#include "usermsg.h"
#include <algorithm>

IMPLEMENT_DYNAMIC(CConvertDialog, CDialogEx)

BEGIN_MESSAGE_MAP(CConvertDialog, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_CUSTOM_CONVERSION_PROGRESS, &CConvertDialog::OnConversionProgress)
	ON_MESSAGE(WM_CUSTOM_CONVERSION_COMPLETED, &CConvertDialog::OnConversionCompleted)
END_MESSAGE_MAP()

CConvertDialog::CConvertDialog(CWnd* pParent) : CDialogEx(IDD_CONVERTPROGRESS, pParent)
{
	
}

void CConvertDialog::UpdateProgress(float progress)
{
	if(CProgressCtrl* pCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_CONVERT_PROGRESS)))
	{
		progress = std::clamp(progress, 0.0f, 1.0f) * 100;
		CString header;
		header.Format(_T("Converting... (%.1f%%)"), progress);
		this->SetWindowText(header);
		int p = static_cast<int>(round(progress));
		pCtrl->SetPos(p + 1);
		pCtrl->SetPos(p);
	}
}

void CConvertDialog::OnOpen()
{
	dialogOpened.SetEvent();
}

void CConvertDialog::OnClose()
{
	opened = false;
}

void CConvertDialog::OnWindowPosChanged(WINDOWPOS* wndPos)
{
	if((wndPos->flags & SWP_SHOWWINDOW) && !opened)
	{
		opened = true;
		OnOpen();
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
