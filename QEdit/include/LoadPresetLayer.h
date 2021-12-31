#pragma once
#include "UILayer.h"

class LoadPresetLayer : public UILayer
{
private:
	std::function<void(std::string)> loadPresetCallback;
	ftxui::Component ui_PresetMenu;

	int selectedPreset = -1;
	//Unfortunately 2 lists are required for the UI to work
	std::vector<std::string> presetPaths;
	std::vector<std::string> presetNames;

public:
	LoadPresetLayer(std::function<void(std::string)> loadPresetCallback) : loadPresetCallback(loadPresetCallback) { }
	std::vector<ftxui::Component> GenComponents() override;
	ftxui::Element Render() override;
	void FindPresets();
};
