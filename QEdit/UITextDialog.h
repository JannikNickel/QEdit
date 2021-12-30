#pragma once
#include "UIDialog.h"
#include <chrono>

class UITextDialog : public UIDialog
{
	std::string title;
	std::string text;
	float duration;
	std::chrono::system_clock::time_point creationTime;

public:
	UITextDialog(std::string title, std::string text, float duration);
	ftxui::Element Render() override;
	bool IsClosed() override;
};

