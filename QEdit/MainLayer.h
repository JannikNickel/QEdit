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
	std::function<void()> encodeCallback;

public:
	MainLayer(OptionCollection options, std::function<void()> encodeCallback) : options(options), encodeCallback(encodeCallback) { }
	std::vector<ftxui::Component> GenComponents() override;
	ftxui::Element Render() override;
};

