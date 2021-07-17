#pragma once
#include <string>
#include <iostream>
#include "OptionType.h"
#include "ConsoleHelper.h"

struct Option
{
	bool enabled = false;
	virtual OptionType GetType() = 0;
	virtual std::string ToString() = 0;
	virtual void ConsoleInput() = 0;
};

