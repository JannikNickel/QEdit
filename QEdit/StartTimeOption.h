#pragma once
#include "Option.h"

struct StartTimeOption : Option
{
	float seconds = true;

	StartTimeOption(float seconds) : seconds(seconds)
	{

	}

	virtual OptionType GetType() override
	{
		return OptionType::Input;
	}

	virtual std::string ToString() override
	{
		return std::string("-ss ") + std::to_string(seconds);
	}
};

