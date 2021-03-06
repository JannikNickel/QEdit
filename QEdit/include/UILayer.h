#pragma once
#include <vector>
#include "UIHelper.h"

class UILayer
{
public:
	bool enabled = false;
	virtual std::vector<ftxui::Component> GenComponents() = 0;
	virtual ftxui::Element Render() = 0;
};

