#pragma once
#include <vector>
#include <functional>
#include <array>

#include "Option.h"
#include "UIDialog.h"
#include "UILayer.h"
#include "MainLayer.h"
#include "LoadPresetLayer.h"
#include "SavePresetLayer.h"

#include "ftxui/screen/screen.hpp"
#include "ftxui/component/screen_interactive.hpp"

class UI
{
private:
	UIDialog* currentDialog = nullptr;
	ftxui::ScreenInteractive* screen;
	int selectedLayer = 0;
	std::array<UILayer*, 3> layers;

public:
	void Run(std::vector<Option*> options, std::function<void()> encodeCallback, std::function<void()> loadPresetCallback, std::function<void()> savePresetCallback);
	void ShowDialog(UIDialog* dialog);
	void ForceRedraw();
};

