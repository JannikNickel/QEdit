#pragma once
#include "Option.h"

struct FramerateOption : Option
{
	unsigned int fps = 60;

	FramerateOption(unsigned int fps) : fps(fps)
	{

	}

	FramerateOption()
	{
		Reset();
	}

	OptionType GetType() override
	{
		return OptionType::Output;
	}

	std::string ToString() override
	{
		return "-r " + std::to_string(fps);
	}

	std::string UICategory() override
	{
		return videoUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Change Framerate", &enabled), ftxui::Input(&ui_FramerateInput, "") };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component> components) override
	{
		return std::vector<ftxui::Element>
		{
			ftxui::hbox(
				ftxui::flex(components[0]->Render()) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, ui_LeftLabelWidth),
				ftxui::flex_shrink(ftxui::text("Fps: ")),
				ftxui::flex(components[1]->Render()) | ftxui::bgcolor(ui_FieldBgColor)
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
			fps = (unsigned int)(std::stoi(ui_FramerateInput));
		}
		catch(const std::exception&)
		{
			fps = 60;
			error = "Invalid video framerate input, expected integer";
			return false;
		}
		return true;
	}

	void Reset() override
	{
		enabled = false;
		fps = 0;
		ui_FramerateInput = "";
	}

private:
	std::string ui_FramerateInput;
};
