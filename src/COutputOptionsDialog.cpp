#include "pch.h"
#include "COutputOptionsDialog.h"
#include "resource.h"
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
	return m_Result;
}

BOOL COutputOptionsDialog::OnInitDialog()
{
	initUIValues = false;
	return CDialogEx::OnInitDialog();
}

void COutputOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CSET_CHECK_RESOLUTION, m_ResCheck);
	DDX_Control(pDX, IDC_CSET_CHECK_FPS, m_FpsCheck);
	DDX_Control(pDX, IDC_CSET_CHECK_BITRATE, m_BitrateCheck);
	DDX_Control(pDX, IDC_CSET_CHECK_CODEC, m_CodecCheck);
	DDX_Control(pDX, IDC_CSET_CHECK_ADMUTE, m_AudioMuteCheck);
	DDX_Control(pDX, IDC_CSET_CHECK_ADBITRATE, m_AudioBitrateCheck);

	DDX_Control(pDX, IDC_CSET_RES_WIDTH, m_ResWidthEdit);
	DDX_Control(pDX, IDC_CSET_RES_HEIGHT, m_ResHeightEdit);
	DDX_Control(pDX, IDC_CSET_FPS, m_FpsEdit);
	DDX_Control(pDX, IDC_CSET_BITRATE, m_BitrateEdit);
	DDX_Control(pDX, IDC_CSET_BITRATE_MODE, m_BitrateModeCombo);
	DDX_Control(pDX, IDC_CSET_CODEC, m_CodecCombo);
	DDX_Control(pDX, IDC_CSET_ADBITRATE, m_AudioBitrateEdit);

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
	m_ResWidthEdit.SetWindowText(_T("1920"));
	m_ResHeightEdit.SetWindowText(_T("1080"));
	m_FpsEdit.SetWindowText(_T("60"));
	m_BitrateEdit.SetWindowText(_T("6000"));
	m_BitrateModeCombo.SetCurSel(0);
	m_CodecCombo.SetCurSel(0);
	m_AudioBitrateEdit.SetWindowText(_T("128"));
}

void COutputOptionsDialog::UpdateControlState()
{
	m_ResWidthEdit.EnableWindow(m_ResCheck.GetCheck());
	m_ResHeightEdit.EnableWindow(m_ResCheck.GetCheck());
	m_FpsEdit.EnableWindow(m_FpsCheck.GetCheck());
	m_BitrateEdit.EnableWindow(m_BitrateCheck.GetCheck());
	m_BitrateModeCombo.EnableWindow(m_BitrateCheck.GetCheck());
	m_CodecCombo.EnableWindow(m_CodecCheck.GetCheck());
	m_AudioBitrateEdit.EnableWindow(m_AudioBitrateCheck.GetCheck());
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
	RestrictEdit(m_ResWidthEdit, 4, 4096);
	RestrictEdit(m_ResHeightEdit, 4, 4096);
	RestrictEdit(m_FpsEdit, 1, 300);
	RestrictEdit(m_BitrateEdit, 128, 32768);
	RestrictEdit(m_AudioBitrateEdit, 16, 1024);
}

void COutputOptionsDialog::ReadResult()
{
	ClampValues();
	m_Result.video =
	{
		.resolution =
		{
			.enabled = static_cast<int8_t>(m_ResCheck.GetCheck()),
			.value =
			{
				GetDlgItemInt(IDC_CSET_RES_WIDTH),
				GetDlgItemInt(IDC_CSET_RES_HEIGHT)
			}
		},
		.fps =
		{
			.enabled = static_cast<int8_t>(m_FpsCheck.GetCheck()),
			.value = GetDlgItemInt(IDC_CSET_FPS)
		},
		.bitrate =
		{
			.enabled = static_cast<int8_t>(m_BitrateCheck.GetCheck()),
			.value =
			{
				.rate = GetDlgItemInt(IDC_CSET_BITRATE),
				.mode = static_cast<BitrateMode>(m_BitrateModeCombo.GetCurSel())
			}
		},
		.codec =
		{
			.enabled = static_cast<int8_t>(m_CodecCheck.GetCheck()),
			.value = static_cast<Codec>(m_CodecCombo.GetCurSel())
		}
	};
	m_Result.audio =
	{
		.mute = static_cast<int8_t>(m_AudioMuteCheck.GetCheck()),
		.bitrate =
		{
			.enabled = static_cast<int8_t>(m_AudioBitrateCheck.GetCheck()),
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
