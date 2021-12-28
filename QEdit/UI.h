#pragma once
#include <vector>
#include <functional>

#include "Option.h"
#include "UIDialog.h"

class UI
{
	UIDialog* currentDialog = nullptr;

public:
	void Run(std::vector<Option*> options, std::function<void()> encodeCallback);
	void ShowDialog(UIDialog* dialog);
};

