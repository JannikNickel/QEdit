#pragma once
#include "UIDialog.h"

class UISavePresetDialog : public UIDialog
{
	static const std::string presetPath;

public:
	ftxui::Element Render() override;
	bool IsClosed() override;
};

