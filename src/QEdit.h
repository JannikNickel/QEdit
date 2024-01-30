#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"

class CQEditApp : public CWinAppEx
{
public:
	CQEditApp() noexcept;

public:
	BOOL InitInstance() override;
	int ExitInstance() override;

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CQEditApp theApp;
