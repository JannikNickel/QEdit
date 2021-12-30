#pragma once
#include "Option.h"

struct StartTimeOption : Option
{
	float seconds = true;

	StartTimeOption(float seconds) : seconds(seconds)
	{

	}

	StartTimeOption()
	{
		Reset();
	}

	OptionType GetType() override
	{
		return OptionType::Input;
	}

	std::string ToString() override
	{
		return std::string("-ss ") + std::to_string(seconds);
	}

	std::string UICategory() override
	{
		return globalUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Change Start Time", &enabled), ftxui::Input(&ui_SecondsInput, "") };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component> components) override
	{
		return std::vector<ftxui::Element>
		{
			ftxui::hbox(
				ftxui::flex(components[0]->Render()) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, ui_LeftLabelWidth),
				ftxui::flex_shrink(ftxui::text("Start time (s): ")),
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
			seconds = std::stof(ui_SecondsInput);
		}
		catch(const std::exception&)
		{
			seconds = 0;
			error = "Invalid video start time input, expected float";
			return false;
		}
		return true;
	}

	void Reset() override
	{
		enabled = false;
		seconds = 0;
		ui_SecondsInput = "";
	}

private:
	std::string ui_SecondsInput;
};

