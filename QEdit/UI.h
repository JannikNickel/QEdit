#pragma once
#include <vector>
#include <functional>

#include "Option.h"
#include "UIDialog.h"

#include "ftxui/screen/screen.hpp"
#include "ftxui/component/screen_interactive.hpp"

class UI
{
private:
	UIDialog* currentDialog = nullptr;
	ftxui::ScreenInteractive* screen;

public:
	void Run(std::vector<Option*> options, std::function<void()> encodeCallback);
	void ShowDialog(UIDialog* dialog);
	void ForceRedraw();
};

