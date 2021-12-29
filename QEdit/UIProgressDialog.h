#pragma once
#include "UIDialog.h"

class UIProgressDialog : public UIDialog
{
	std::string title = "Progress";
	float progress = 0.0f;
	bool canceled = false;

public:
	UIProgressDialog(std::string title) : title(title) { }
	void SetProgress(float progress);
	ftxui::Element Render() override;
	bool IsClosed() override;
};

