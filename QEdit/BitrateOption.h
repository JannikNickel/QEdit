#pragma once
#include "Option.h"

struct BitrateOption : Option
{
	/// <summary>
	/// Bitrate in kbit/s
	/// </summary>
	unsigned int bitrate = 0;
	//VBR = -b:v 0k (recommended), -minrate 0k (min), -maxrate 0k (max)
	int mode = 0;//0 = CBR, 1 = VBR

	BitrateOption(unsigned int bitrate) : bitrate(bitrate)
	{

	}

	BitrateOption()
	{
		Reset();
	}

	OptionType GetType() override
	{
		return OptionType::Output;
	}

	std::string ToString() override
	{
		if(mode == 1)
		{
			return "-b:v " + std::to_string(bitrate) + "k -minrate " + std::to_string(bitrate) + "k -maxrate " + std::to_string(bitrate) + "k";
		}
		return "-b:v " + std::to_string(bitrate) + "k";
	}

	std::string UICategory() override
	{
		return videoUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Change Bitrate", &enabled), ftxui::Input(&ui_BitrateInput, ""), ftxui::Radiobox(&ui_ModeOptions, &mode) };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component> components) override
	{
		if(enabled)
		{
			return std::vector<ftxui::Element>
			{
				ftxui::vbox(
					ftxui::hbox(
						ftxui::flex(components[0]->Render()) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, ui_LeftLabelWidth),
						ftxui::flex_shrink(ftxui::text("Bitrate (kbit/s): ")),
						ftxui::flex(components[1]->Render()) | ftxui::bgcolor(ui_FieldBgColor)
					),
					ftxui::hbox(
						ftxui::flex(ftxui::text("")) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, ui_LeftLabelWidth + 1),
						ftxui::flex(components[2]->Render())
					)
				)
			};
		}
		return std::vector<ftxui::Element>
		{
			ftxui::vbox(ftxui::flex(components[0]->Render()) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, ui_LeftLabelWidth))
		};
	}

	bool ReadUIValues(std::string& error) override
	{
		//enabled is already set
		if(enabled == false)
		{
			return true;
		}

		//mode is already set
		try
		{
			bitrate = (unsigned int)(std::stoi(ui_BitrateInput));
		}
		catch(const std::exception&)
		{
			bitrate = 0;
			error = "Invalid video bitrate input, expected integer";
			return false;
		}
		return true;
	}

	void Reset() override
	{
		enabled = false;
		bitrate = 0;
		mode = 0;
		ui_BitrateInput = "";
	}

	void SavePreset(CFGWriter& writer) override
	{
		writer.Write("videoBitrateF", std::to_string(enabled));
		writer.Write("videoBitrate", std::to_string(bitrate));
		writer.Write("videoBitrateM", std::to_string(mode));
	}

	void LoadPreset(CFGReader& reader) override
	{
		enabled = reader.Read("videoBitrateF") == "1";
		ui_BitrateInput = reader.Read("videoBitrate");
		try
		{
			mode = std::stoi(reader.Read("videoBitrateM"));
		}
		catch(const std::exception&) { }
	}

private:
	std::string ui_BitrateInput;
	const std::vector<std::string> ui_ModeOptions = { "CBR", "VBR" };
};
