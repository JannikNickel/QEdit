#pragma once
#include "Option.h"

struct CodecOption : Option
{
	int selectedCodec = 0;

	CodecOption()
	{
		Reset();
	}

	OptionType GetType() const override
	{
		return OptionType::Output;
	}

	std::string ToString() const override
	{
		return "-c:v " + codecCmdName[selectedCodec];
	}

	std::string UICategory() const override
	{
		return videoUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Change Codec", &enabled), ftxui::Radiobox(&ui_CodecOptions, &selectedCodec) };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component>& components) const override
	{
		return std::vector<ftxui::Element>
		{
			ftxui::vbox(
				ftxui::hbox(
					ftxui::flex(components[0]->Render()) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, ui_LeftLabelWidth),
					enabled ? ftxui::flex(components[1]->Render()) : ftxui::text("")
				)
			)
		};
	}

	bool ReadUIValues(std::string& error) override
	{
		//enabled already set
		//selectedCodec already set
		return true;
	}

	void Reset() override
	{
		enabled = false;
		selectedCodec = 0;
	}

	void SavePreset(CFGWriter& writer) const override
	{
		writer.Write("videoCodecF", std::to_string(enabled));
		writer.Write("videoCodec", std::to_string(selectedCodec));
	}

	void LoadPreset(CFGReader& reader) override
	{
		enabled = reader.Read("videoCodecF") == "1";
		try
		{
			selectedCodec = std::stoi(reader.Read("videoCodec"));
		}
		catch(const std::exception&) { }
	}

private:
	std::vector<std::string> ui_CodecOptions = { "H264", "H265 (HEVC)", "MPEG4" };
	std::vector<std::string> codecCmdName = { "h264_mf", "h265_nvenc", "mpeg4" };
};
