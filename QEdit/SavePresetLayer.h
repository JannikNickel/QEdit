#pragma once
#include "UILayer.h"

class SavePresetLayer : public UILayer
{
public:
	std::vector<ftxui::Component> GenComponents() override;
	ftxui::Element Render() override;
};

