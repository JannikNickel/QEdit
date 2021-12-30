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

void UI::Run(std::vector<Option*> options, std::function<void()> encodeCallback, std::function<void()> loadPresetCallback, std::function<void()> savePresetCallback)
{
	layers = { new MainLayer(options), new LoadPresetLayer(), new SavePresetLayer() };

	ftxui::Components components;
	for(UILayer* layer : layers)
	{
		std::vector<ftxui::Component> comps = layer->GenComponents();
		components.insert(components.end(), comps.begin(), comps.end());
	}

	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::TerminalOutput();
	this->screen = &screen;

	std::vector<std::string> menuItems = { "QEdit", "Load Preset", "Save Preset" };
	ftxui::Component tab = ftxui::Toggle(&menuItems, &selectedLayer);
	components.push_back(tab);

	ftxui::Component container = ftxui::Container::Vertical(components);
	ftxui::Component renderer = ftxui::Renderer(container, [&]
	{
		std::vector<ftxui::Element> elements;
		elements.push_back(tab->Render());
		elements.push_back(ftxui::separator());
		elements.push_back(layers[selectedLayer]->Render());

		ftxui::Element mainElement = vbox(elements);

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
