#pragma once
#include "Option.h"

struct DurationOption : Option
{
	float seconds = true;

	DurationOption(float seconds) : seconds(seconds)
	{

	}

	DurationOption()
	{
		Reset();
	}

	OptionType GetType() const override
	{
		return OptionType::Output;
	}

	std::string ToString() const override
	{
		return std::string("-t ") + std::to_string(seconds);
	}

	std::string UICategory() const override
	{
		return globalUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Change Duration", &enabled), ftxui::Input(&ui_SecondsInput, "") };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component>& components) const override
	{
		return std::vector<ftxui::Element>
		{
			ftxui::hbox(
				ftxui::flex(components[0]->Render()) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, ui_LeftLabelWidth),
				enabled ? ftxui::flex_shrink(ftxui::text("Duration (s): ")) : ftxui::text(""),
				enabled ? ftxui::flex(components[1]->Render()) | ftxui::bgcolor(ui_FieldBgColor) : ftxui::text("")
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
			error = "Invalid video duration input, expected float";
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

	void SavePreset(CFGWriter& writer) const override
	{
		writer.Write("durationF", std::to_string(enabled));
		writer.Write("duration", std::to_string(seconds));
	}

	void LoadPreset(CFGReader& reader) override
	{
		enabled = reader.Read("durationF") == "1";
		ui_SecondsInput = reader.Read("duration");
	}

private:
	std::string ui_SecondsInput;
};

