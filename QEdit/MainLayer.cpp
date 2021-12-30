#include "MainLayer.h"

std::vector<ftxui::Component> MainLayer::GenComponents()
{
	std::vector<ftxui::Component> components;

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

	encodeButton = ftxui::Button("[Encode]", NULL, DefaultButtonOption());
	components.push_back(encodeButton);

	return components;
}

ftxui::Element MainLayer::Render()
{
	std::vector<ftxui::Element> elements;

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
		elements.push_back(window);
	}

	elements.push_back(ftxui::hbox(ftxui::flex_shrink(encodeButton->Render())));

	return ftxui::vbox(elements);
}
