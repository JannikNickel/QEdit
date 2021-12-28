#include "UIProgressDialog.h"
#include <cmath>

ftxui::Element UIProgressDialog::Render()
{
	std::string header = this->title + " " + std::to_string(std::roundf(this->progress * 100.0f)) + "%";
	return ftxui::border(ftxui::vbox(
		ftxui::bold(ftxui::text(header)),
		ftxui::separator(),
		ftxui::flex(ftxui::gauge(this->progress)) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 50)
	));
}

bool UIProgressDialog::IsClosed()
{
	return progress >= 1.0f || canceled;
}
