#pragma once
#include "Option.h"

struct OutputFileOption : Option
{
	std::string url = "";

	OutputFileOption(std::string url) : OutputFileOption()
	{
		this->url = url;
	}

	OutputFileOption()
	{
		Reset();
	}

	OptionType GetType() const override
	{
		return OptionType::OutputFile;
	}

	std::string ToString() const override
	{
		return "\"" + url + "\"";
	}

	std::string UICategory() const override
	{
		return outputUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Input(&ui_UrlInput, "") };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component>& components) const override
	{
		return std::vector<ftxui::Element>
		{
			ftxui::hbox(
				ftxui::flex_shrink(ftxui::text("Output file: ")),
				ftxui::flex(components[0]->Render()) | ftxui::bgcolor(ui_FieldBgColor)
			)
		};
	}

	bool ReadUIValues(std::string& error) override
	{
		url = ui_UrlInput;
		return true;
	}

	void Reset() override
	{
		enabled = true;
		url = "";
		ui_UrlInput = "";
	}

	void SavePreset(CFGWriter& writer) const override
	{

	}

	void LoadPreset(CFGReader& reader) override
	{

	}

private:
	std::string ui_UrlInput = "";
};
