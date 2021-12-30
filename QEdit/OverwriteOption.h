#pragma once
#include "Option.h"

struct OverwriteOption : Option
{
	OverwriteOption()
	{
		Reset();
	}

	OptionType GetType() override
	{
		return OptionType::Global;
	}

	std::string ToString() override
	{
		return std::string("-") + (enabled == true ? "y" : "n");
	}

	std::string UICategory() override
	{
		return outputUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Overwrite Output", &enabled) };
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
		enabled = true;
	}

	void SavePreset(CFGWriter& writer) override
	{
		writer.Write("overwriteF", std::to_string(enabled));
	}

	void LoadPreset(CFGReader& reader) override
	{
		enabled = reader.Read("overwriteF") == "1";
	}
};

