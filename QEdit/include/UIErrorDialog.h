#pragma once
#include "UIDialog.h"

#include <vector>
#include <string>
#include <chrono>

class UIErrorDialog : public UIDialog
{
	std::vector<std::string> errors;
	std::chrono::system_clock::time_point creationTime;
	const float visibilityDuration = 2.0f;

public:
	UIErrorDialog(std::vector<std::string> errors);
	ftxui::Element Render() override;
	bool IsClosed() override;
};

