#include "UIErrorDialog.h"

UIErrorDialog::UIErrorDialog(std::vector<std::string> errors) : errors(errors)
{
	creationTime = std::chrono::system_clock::now();
}

ftxui::Element UIErrorDialog::Render()
{
	std::vector<ftxui::Element> elements;
	elements.push_back(ftxui::bold(ftxui::text("ERROR")));
	elements.push_back(ftxui::separator());
	for(std::string error : errors)
	{
		elements.push_back(ftxui::text(error));
	}
	return ftxui::border(ftxui::vbox(elements));
}

bool UIErrorDialog::IsClosed()
{
	std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - creationTime;
	return elapsed.count() > visibilityDuration;
}
