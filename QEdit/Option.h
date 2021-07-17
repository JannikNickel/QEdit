#pragma once
#include <string>
#include "OptionType.h"

struct Option
{
	virtual OptionType GetType() = 0;

	virtual std::string ToString() = 0;
};

