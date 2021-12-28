#pragma once
#include "UIHelper.h"

class UIDialog
{
public:
	UIDialog() { }
	~UIDialog() { }
	virtual ftxui::Element Render() = 0;
	virtual bool IsClosed() = 0;
};