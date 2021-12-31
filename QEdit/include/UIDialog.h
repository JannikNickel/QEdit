#pragma once
#include "UIHelper.h"
#include <mutex>

class UIDialog
{
public:
	UIDialog() { }
	~UIDialog() { }
	virtual ftxui::Element Render() = 0;
	virtual bool IsClosed() = 0;
};

static std::mutex uiDialogMutex = std::mutex();
