#include "LoadPresetLayer.h"
#include "Settings.h"
#include <filesystem>
#include <Windows.h>
#pragma comment(lib, "User32.lib")

bool lmb = false;

std::vector<ftxui::Component> LoadPresetLayer::GenComponents()
{
	ftxui::MenuOption menuOption;
	menuOption.on_enter = [&]
	{
		if(enabled)
		{
			loadPresetCallback(presetPaths[selectedPreset]);
		}
	};
	menuOption.on_change = [&]
	{
		//Only react if left mouse button was pressed during last frame
		if(enabled && lmb)
		{
			loadPresetCallback(presetPaths[selectedPreset]);
		}
	};
	ui_PresetMenu = ftxui::Menu(&presetNames, &selectedPreset, menuOption);
	return std::vector<ftxui::Component>
	{
		ui_PresetMenu
	};
}

ftxui::Element LoadPresetLayer::Render()
{
	lmb = GetAsyncKeyState(VK_LBUTTON);
	return ui_PresetMenu->Render();
}

void LoadPresetLayer::FindPresets()
{
	selectedPreset = -1;
	presetPaths.clear();
	presetNames.clear();
	std::filesystem::path p = GetWorkingDirectory() + presetPath;
	if(std::filesystem::exists(p))
	{
		for(const auto& file : std::filesystem::directory_iterator(p))
		{
			std::filesystem::path preset = file;
			if(std::filesystem::is_regular_file(preset))
			{
				presetPaths.push_back(preset.string());
				presetNames.push_back(preset.filename().string());
			}
		}
	}
}
