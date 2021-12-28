#pragma once
#include <string>
#include <iostream>
#include <vector>

#include "OptionType.h"
#include "ConsoleHelper.h"
#include "UIHelper.h"

#include "ftxui/dom/elements.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"

struct Option
{
	bool enabled = false;
	virtual OptionType GetType() = 0;
	virtual std::string ToString() = 0;

	virtual std::string UICategory() = 0;
	virtual std::vector<ftxui::Component> GenUIComponents() = 0;
	virtual std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component> components) = 0;
	virtual bool ReadUIValues(std::string& error) = 0;
};

static const std::string inputUICategory = "Input";
static const std::string globalUICategory = "Global";
static const std::string videoUICategory = "Video";
static const std::string audioUICategory = "Audio";
static const std::string outputUICategory = "Output";
static const std::string uiCategoryOrder[5] = { inputUICategory, globalUICategory, videoUICategory, audioUICategory, outputUICategory };
