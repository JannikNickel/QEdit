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

	OptionType GetType() const override
	{
		return OptionType::Output;
	}

	std::string ToString() const override
	{
		return "-r " + std::to_string(fps);
	}

	std::string UICategory() const override
	{
		return videoUICategory;
	}

	std::vector<ftxui::Component> GenUIComponents() override
	{
		return std::vector<ftxui::Component> { ftxui::Checkbox("Change Framerate", &enabled), ftxui::Input(&ui_FramerateInput, "") };
	}

	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component>& components) const override
	{
		return std::vector<ftxui::Element>
		{
			ftxui::hbox(
				ftxui::flex(components[0]->Render()) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, ui_LeftLabelWidth),
				enabled ? ftxui::flex_shrink(ftxui::text("Fps: ")) : ftxui::text(""),
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

	void SavePreset(CFGWriter& writer) const override
	{
		writer.Write("videoFpsF", std::to_string(enabled));
		writer.Write("videoFps", std::to_string(fps));
	}

	void LoadPreset(CFGReader& reader) override
	{
		enabled = reader.Read("videoFpsF") == "1";
		ui_FramerateInput = reader.Read("videoFps");
	}

private:
	std::string ui_FramerateInput;
};
