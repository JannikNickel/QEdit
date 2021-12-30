#pragma once
#include "Option.h"

struct MuteOption : Option
{
	MuteOption()
	{
		Reset();
	}

	OptionType GetType() override
	{
		return OptionType::Global;
	}

	std::string ToString() override
	{
		return "-an";
	}

	std::string UICategory() override
	{
		return audioUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Mute Audio", &enabled) };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component> components) override
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
};

