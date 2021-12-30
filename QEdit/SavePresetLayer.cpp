#include "SavePresetLayer.h"

std::vector<ftxui::Component> SavePresetLayer::GenComponents()
{
	ui_nameInput = ftxui::Input(&nameInput, "");
	ui_saveButton = ftxui::Button("[Save]", [&]
	{
		savePresetCallback(nameInput);
	}, DefaultButtonOption());
	return std::vector<ftxui::Component>
	{
		ui_nameInput,
		ui_saveButton
	};
}

ftxui::Element SavePresetLayer::Render()
{
	return ftxui::vbox(
		ftxui::hbox(
			ftxui::flex_shrink(ftxui::text("Preset name: ")),
			ftxui::flex(ui_nameInput->Render()) | ftxui::bgcolor(ui_FieldBgColor)
		),
		ftxui::text(""),
		ftxui::hbox(ftxui::flex_shrink(ui_saveButton->Render()))
	);
}
