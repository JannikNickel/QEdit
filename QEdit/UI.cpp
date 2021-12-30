#include "UI.h"
#include "Option.h"
#include "UIHelper.h"
#include "UIProgressDialog.h"

#include <vector>
#include <map>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"

void UI::Run(std::vector<Option*> options, std::function<void()> encodeCallback)
{
	ftxui::Components components;
	std::map<Option*, std::vector<ftxui::Component>> optionComponents;

	for(std::string category : uiCategoryOrder)
	{
		for(Option* op : options)
		{
			if(op != nullptr && op->UICategory() == category)
			{
				std::vector<ftxui::Component> comps = op->GenUIComponents();
				std::vector<ftxui::Element> dom = op->GetUIDom(comps);

				components.insert(components.end(), comps.begin(), comps.end());
				optionComponents.insert(std::pair<Option*, std::vector<ftxui::Component>>(op, comps));
			}
		}
	}

	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::TerminalOutput();
	this->screen = &screen;

	ftxui::Component encodeButton = ftxui::Button("[Encode]", encodeCallback, DefaultButtonOption());
	components.push_back(encodeButton);
	ftxui::Component loadPresetButton = ftxui::Button("[Load Preset]", [&]{}, DefaultButtonOption());
	components.push_back(loadPresetButton);
	ftxui::Component savePresetButton = ftxui::Button("[Save Preset]", [&]{}, DefaultButtonOption());
	components.push_back(savePresetButton);

	ftxui::Component container = ftxui::Container::Vertical(components);
	ftxui::Component renderer = ftxui::Renderer(container, [&]
	{
		std::vector<ftxui::Element> windows;

		//Header
		ftxui::Element titlebar = ftxui::hbox(ftxui::color(ftxui::Color::Aquamarine1, ftxui::text("QEdit")), ftxui::text("  "), loadPresetButton->Render(), ftxui::text("  "), savePresetButton->Render());
		windows.push_back(titlebar);
		windows.push_back(ftxui::separator());

		for(std::string category : uiCategoryOrder)
		{
			std::vector<ftxui::Element> windowContent;
			for(Option* op : options)
			{
				if(op != nullptr && op->UICategory() == category)
				{
					std::vector<ftxui::Element> elements = op->GetUIDom(optionComponents[op]);
					windowContent.insert(windowContent.end(), elements.begin(), elements.end());
				}
			}
			if(windowContent.empty() == true)
			{
				continue;
			}

			ftxui::Element window = ftxui::window(ftxui::text(category), ftxui::vbox(windowContent));
			windows.push_back(window);
		}

		windows.push_back(ftxui::hbox(ftxui::flex_shrink(encodeButton->Render())));

		ftxui::Element mainElement = ftxui::vbox(windows);

		uiDialogMutex.lock();
		if(currentDialog != nullptr)
		{
			if(currentDialog->IsClosed() == false)
			{
				ftxui::Element dialogContent = currentDialog->Render();
				mainElement = ftxui::dbox({ ftxui::color(ftxui::Color::GrayDark, mainElement), dialogContent | ftxui::clear_under | ftxui::center });
			}
		}
		uiDialogMutex.unlock();

		return mainElement;
	});

	screen.Loop(renderer);
}

void UI::ShowDialog(UIDialog* dialog)
{
	uiDialogMutex.lock();
	if(currentDialog != nullptr)
	{
		delete currentDialog;
	}
	currentDialog = dialog;
	uiDialogMutex.unlock();
}

void UI::ForceRedraw()
{
	screen->PostEvent(ftxui::Event::Custom);
}
