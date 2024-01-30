#include "pch.h"
#include "resource.h"
#include "OutputOptionsDialog.h"
#include <cmath>
#include <algorithm>

IMPLEMENT_DYNAMIC(COutputOptionsDialog, CDialogEx)

BEGIN_MESSAGE_MAP(COutputOptionsDialog, CDialogEx)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CSET_CHECK_RESOLUTION, IDC_CSET_CHECK_ADBITRATE, &COutputOptionsDialog::CheckboxChanged)
	ON_BN_CLICKED(IDOK, &COutputOptionsDialog::OnOK)
	ON_EN_KILLFOCUS(IDC_CSET_RES_HEIGHT, &COutputOptionsDialog::ClampValues)
	ON_EN_KILLFOCUS(IDC_CSET_RES_WIDTH, &COutputOptionsDialog::ClampValues)
	ON_EN_KILLFOCUS(IDC_CSET_FPS, &COutputOptionsDialog::ClampValues)
	ON_EN_KILLFOCUS(IDC_CSET_BITRATE, &COutputOptionsDialog::ClampValues)
	ON_EN_KILLFOCUS(IDC_CSET_ADBITRATE, &COutputOptionsDialog::ClampValues)
END_MESSAGE_MAP()

COutputOptionsDialog::COutputOptionsDialog(VideoHandle* vidHandle, CWnd* pParent) : CDialogEx(IDD_OUTPUTOPTIONS, pParent), vidHandle(vidHandle)
{

}

OutputSettings COutputOptionsDialog::Result() const
{
	return result;
}

BOOL COutputOptionsDialog::OnInitDialog()
{
	initUIValues = false;
	return CDialogEx::OnInitDialog();
}

void COutputOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CSET_CHECK_RESOLUTION, resBtn);
	DDX_Control(pDX, IDC_CSET_CHECK_FPS, fpsBtn);
	DDX_Control(pDX, IDC_CSET_CHECK_BITRATE, bitrateBtn);
	DDX_Control(pDX, IDC_CSET_CHECK_CODEC, codecBtn);
	DDX_Control(pDX, IDC_CSET_CHECK_ADMUTE, audioMuteBtn);
	DDX_Control(pDX, IDC_CSET_CHECK_ADBITRATE, audioBitrateBtn);

	DDX_Control(pDX, IDC_CSET_RES_WIDTH, resWidthEdit);
	DDX_Control(pDX, IDC_CSET_RES_HEIGHT, resHeightEdit);
	DDX_Control(pDX, IDC_CSET_FPS, fpsEdit);
	DDX_Control(pDX, IDC_CSET_BITRATE, bitrateEdit);
	DDX_Control(pDX, IDC_CSET_BITRATE_MODE, bitrateModeCombo);
	DDX_Control(pDX, IDC_CSET_CODEC, codecCombo);
	DDX_Control(pDX, IDC_CSET_ADBITRATE, audioBitrateEdit);

	if(!initUIValues)
	{
		initUIValues = true;
		SetVideoDataToUI();
	}

	UpdateControlState();
}

void COutputOptionsDialog::SetVideoDataToUI()
{
	//TODO this could show the settings from the vidHandle instead of default values
	resWidthEdit.SetWindowText(_T("1920"));
	resHeightEdit.SetWindowText(_T("1080"));
	fpsEdit.SetWindowText(_T("60"));
	bitrateEdit.SetWindowText(_T("6000"));
	bitrateModeCombo.SetCurSel(0);
	codecCombo.SetCurSel(0);
	audioBitrateEdit.SetWindowText(_T("128"));
}

void COutputOptionsDialog::UpdateControlState()
{
	resWidthEdit.EnableWindow(resBtn.GetCheck());
	resHeightEdit.EnableWindow(resBtn.GetCheck());
	fpsEdit.EnableWindow(fpsBtn.GetCheck());
	bitrateEdit.EnableWindow(bitrateBtn.GetCheck());
	bitrateModeCombo.EnableWindow(bitrateBtn.GetCheck());
	codecCombo.EnableWindow(codecBtn.GetCheck());
	audioBitrateEdit.EnableWindow(audioBitrateBtn.GetCheck());
}

static void RestrictEdit(CEdit& edit, int min, int max)
{
	CString text;
	edit.GetWindowText(text);
	if(!text.IsEmpty())
	{
		int maxLen = max != 0 ? static_cast<int>(std::floor(std::log10(max)) + 1) : 1;
		if(text.GetLength() > maxLen)
		{
			text.Format(_T("%d"), max);
			edit.SetWindowText(text);
			return;
		}
		int value;
		_stscanf_s(text, _T("%d"), &value);
		text.Format(_T("%d"), std::clamp(value, min, max));
		edit.SetWindowText(text);
	}
}

void COutputOptionsDialog::ClampValues()
{
	RestrictEdit(resWidthEdit, 4, 4096);
	RestrictEdit(resHeightEdit, 4, 4096);
	RestrictEdit(fpsEdit, 1, 300);
	RestrictEdit(bitrateEdit, 128, 32768);
	RestrictEdit(audioBitrateEdit, 16, 1024);
}

void COutputOptionsDialog::ReadResult()
{
	ClampValues();
	result.video =
	{
		.resolution =
		{
			.enabled = static_cast<int8_t>(resBtn.GetCheck()),
			.value =
			{
				GetDlgItemInt(IDC_CSET_RES_WIDTH),
				GetDlgItemInt(IDC_CSET_RES_HEIGHT)
			}
		},
		.fps =
		{
			.enabled = static_cast<int8_t>(fpsBtn.GetCheck()),
			.value = GetDlgItemInt(IDC_CSET_FPS)
		},
		.bitrate =
		{
			.enabled = static_cast<int8_t>(bitrateBtn.GetCheck()),
			.value =
			{
				.rate = GetDlgItemInt(IDC_CSET_BITRATE),
				.mode = static_cast<BitrateMode>(bitrateModeCombo.GetCurSel())
			}
		},
		.codec =
		{
			.enabled = static_cast<int8_t>(codecBtn.GetCheck()),
			.value = static_cast<Codec>(codecCombo.GetCurSel())
		}
	};
	result.audio =
	{
		.mute = static_cast<int8_t>(audioMuteBtn.GetCheck()),
		.bitrate =
		{
			.enabled = static_cast<int8_t>(audioBitrateBtn.GetCheck()),
			.value = GetDlgItemInt(IDC_CSET_ADBITRATE)
		}
	};
}

void COutputOptionsDialog::CheckboxChanged(UINT nID)
{
	UpdateControlState();
}

void COutputOptionsDialog::OnOK()
{
	ReadResult();
	CDialogEx::OnOK();
}
