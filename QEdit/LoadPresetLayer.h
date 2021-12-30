#pragma once
#include "UILayer.h"

class LoadPresetLayer : public UILayer
{
public:
	std::vector<ftxui::Component> GenComponents() override;
	ftxui::Element Render() override;
};

