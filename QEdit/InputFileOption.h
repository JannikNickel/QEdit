#pragma once
#include "Option.h"
#include <filesystem>

struct InputFileOption : Option
{
	std::string url = "";

	InputFileOption(std::string url) : InputFileOption()
	{
		this->url = url;
	}

	InputFileOption()
	{
		enabled = true;
	}

	OptionType GetType() override
	{
		return OptionType::InputFile;
	}

	std::string ToString() override
	{
		return "-i \"" + url + "\"";
	}

	std::string UICategory() override
	{
		return inputUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Input(&url, "") };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component> components) override
	{
		return std::vector<ftxui::Element>
		{
			ftxui::hbox(
				ftxui::flex_shrink(ftxui::text("Input file: ")),
				ftxui::flex(components[0]->Render()) | ftxui::bgcolor(ui_FieldBgColor)
			)
		};
	}

	bool ReadUIValues(std::string& error) override
	{
		return true;
	}
};
