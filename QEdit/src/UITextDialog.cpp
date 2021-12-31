#include "UITextDialog.h"

UITextDialog::UITextDialog(std::string title, std::string text, float duration) : title(title), text(text), duration(duration)
{
	creationTime = std::chrono::system_clock::now();
}

ftxui::Element UITextDialog::Render()
{
	return ftxui::border(ftxui::vbox(ftxui::bold(ftxui::text(title)), ftxui::separator(), ftxui::text(text)));
}

bool UITextDialog::IsClosed()
{
	std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - creationTime;
	return elapsed.count() > duration;
}
