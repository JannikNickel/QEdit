#pragma once
#include "Option.h"

struct OutputFileOption : Option
{
	std::string url = "";

	OutputFileOption(std::string url) : url(url)
	{

	}

	OutputFileOption()
	{
		
	}

	OptionType GetType() override
	{
		return OptionType::OutputFile;
	}

	std::string ToString() override
	{
		return "\"" + url + "\"";
	}

	std::string UICategory() override
	{
		return outputUICategory;
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
				ftxui::flex_shrink(ftxui::text("Output file: ")),
				ftxui::flex(components[0]->Render()) | ftxui::bgcolor(ui_FieldBgColor)
			)
		};
	}

	bool ReadUIValues(std::string& error) override
	{
		return true;
	}
};
