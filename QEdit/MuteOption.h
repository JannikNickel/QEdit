#pragma once
#include "Option.h"

struct MuteOption : Option
{
	MuteOption()
	{
		Reset();
	}

	OptionType GetType() const override
	{
		return OptionType::Global;
	}

	std::string ToString() const override
	{
		return "-an";
	}

	std::string UICategory() const override
	{
		return audioUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Mute Audio", &enabled) };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component>& components) const override
	{
		return std::vector<ftxui::Element>
		{
			ftxui::hbox(
				ftxui::flex(components[0]->Render()) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, ui_LeftLabelWidth)
			)
		};
	}

	bool ReadUIValues(std::string& error) override
	{
		//enabled is already set
		return true;
	}

	void Reset() override
	{
		enabled = false;
	}

	void SavePreset(CFGWriter& writer) const override
	{
		writer.Write("muteF", std::to_string(enabled));
	}

	void LoadPreset(CFGReader& reader) override
	{
		enabled = reader.Read("muteF") == "1";
	}
};

