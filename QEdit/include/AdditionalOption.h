#pragma once
#include "Option.h"

struct AdditionalOption : Option
{
	std::string UICategory() const override { return ""; }
	std::vector<ftxui::Component> GenUIComponents() override { return std::vector<ftxui::Component>(); }
	std::vector<ftxui::Element> GetUIDom(std::vector<ftxui::Component>& components) const override { return std::vector<ftxui::Element>(); }
	bool ReadUIValues(std::string& error) override { return true; }
	void Reset() override { enabled = true; }
	void SavePreset(CFGWriter& writer) const override { }
	void LoadPreset(CFGReader& reader) override { }
};

static bool IsAdditionalOption(Option* op)
{
	return dynamic_cast<AdditionalOption*>(op) != nullptr;
}
