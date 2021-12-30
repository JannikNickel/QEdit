#pragma once
#include "Option.h"

struct AudioBitrateOption : Option
{
	/// <summary>
	/// Bitrate in kbit/s
	/// </summary>
	unsigned int bitrate = 0;

	AudioBitrateOption(unsigned int bitrate) : bitrate(bitrate)
	{

	}

	AudioBitrateOption()
	{
		Reset();
	}

	OptionType GetType() override
	{
		return OptionType::Output;
	}

	std::string ToString() override
	{
		return "-b:a " + std::to_string(bitrate) + "k";
	}

	std::string UICategory() override
	{
		return audioUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Change Audio Bitrate", &enabled), ftxui::Input(&ui_BitrateInput, "") };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component> components) override
	{
		return std::vector<ftxui::Element>
		{
			ftxui::hbox(
				ftxui::flex(components[0]->Render()) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, ui_LeftLabelWidth),
				ftxui::flex_shrink(ftxui::text("Bitrate (kbit/s): ")),
				ftxui::flex(components[1]->Render()) | ftxui::bgcolor(ui_FieldBgColor) | ftxui::bgcolor(ui_FieldBgColor)
			)
		};
	}

	bool ReadUIValues(std::string& error) override
	{
		if(enabled == false)
		{
			return true;
		}
		try
		{
			bitrate = (unsigned int)(std::stoi(ui_BitrateInput));
		}
		catch(const std::exception&)
		{
			bitrate = 0;
			error = "Invalid audio bitrate input, expected integer value";
			return false;
		}
		return true;
	}

	void Reset() override
	{
		enabled = false;
		bitrate = 0;
		ui_BitrateInput = "";
	}

private:
	std::string ui_BitrateInput;
};
