#pragma once
#include "Option.h"

struct ResolutionOption : Option
{
	unsigned int width = 0;
	unsigned int height = 0;

	ResolutionOption(unsigned int width, unsigned int height) : width(width), height(height)
	{

	}

	ResolutionOption()
	{
		Reset();
	}

	OptionType GetType() override
	{
		return OptionType::Output;
	}

	std::string ToString() override
	{
		return "-s " + std::to_string(width) + "x" + std::to_string(height);
	}

	std::string UICategory() override
	{
		return videoUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Change Resolution", &enabled), ftxui::Input(&ui_WidthInput, ""), ftxui::Input(&ui_HeightInput, "") };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component> components) override
	{
		return std::vector<ftxui::Element>
		{
			ftxui::hbox(
				ftxui::flex(components[0]->Render()) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 50),
				enabled ? ftxui::flex_shrink(ftxui::text("Resolution: ")) : ftxui::text(""),
				enabled ? ftxui::flex_shrink(components[1]->Render()) | ftxui::bgcolor(ui_FieldBgColor) : ftxui::text(""),
				enabled ? ftxui::flex_shrink(ftxui::text("x")) : ftxui::text(""),
				enabled ? ftxui::flex_shrink(components[2]->Render()) | ftxui::bgcolor(ui_FieldBgColor) : ftxui::text("")
			)
		};
	}

	bool ReadUIValues(std::string& error) override
	{
		//enabled is already set
		if(enabled == false)
		{
			return true;
		}
		try
		{
			width = (unsigned int)(std::stoi(ui_WidthInput));
			height = (unsigned int)(std::stoi(ui_HeightInput));
		}
		catch(const std::exception&)
		{
			width = 0;
			height = 0;
			error = "Invalid resolution input, expected integer";
			return false;
		}
		return true;
	}

	void Reset() override
	{
		enabled = false;
		width = 0;
		height = 0;
		ui_WidthInput = "";
		ui_HeightInput = "";
	}

private:
	std::string ui_WidthInput;
	std::string ui_HeightInput;
};
