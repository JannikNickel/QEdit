#include "UIProgressDialog.h"
#include <cmath>
#include <cstdio>

void UIProgressDialog::SetProgress(float p)
{
	if(p < 0.0f)
	{
		p = 0.0f;
	}
	else if(p > 1.0f)
	{
		p = 1.0f;
	}
	this->progress = p;
}

ftxui::Element UIProgressDialog::Render()
{
	char buffer[7];
	sprintf_s(buffer, "%.2f", this->progress * 100.0);
	std::string header = this->title + " " + buffer + "%";
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
