#pragma once
#include "UILayer.h"
#include "Option.h"

#include <map>

class MainLayer : public UILayer
{
private:
	OptionCollection options;
	std::map<Option*, std::vector<ftxui::Component>> optionComponents;
	ftxui::Component encodeButton;

public:
	MainLayer(OptionCollection options) : options(options) { }
	std::vector<ftxui::Component> GenComponents() override;
	ftxui::Element Render() override;
};

