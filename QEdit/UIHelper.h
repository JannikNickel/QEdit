#pragma once
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"

static ftxui::ButtonOption DefaultButtonOption()
{
	ftxui::ButtonOption option;
	option.border = false;
	return option;
}

static const float ui_LeftLabelWidth = 50;
static const ftxui::Color ui_FieldBgColor = ftxui::Color::RGB(50, 50, 50);//ftxui::Color::GrayDark;
