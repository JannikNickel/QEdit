#pragma once
#include "UILayer.h"

class SavePresetLayer : public UILayer
{
private:
	std::string nameInput;
	std::function<void(std::string)> savePresetCallback;
	ftxui::Component ui_nameInput;
	ftxui::Component ui_saveButton;

public:
	SavePresetLayer(std::function<void(std::string)> savePresetCallback) : savePresetCallback(savePresetCallback) { }
	std::vector<ftxui::Component> GenComponents() override;
	ftxui::Element Render() override;
};

