#pragma once
#include "Option.h"

struct DurationOption : Option
{
	float seconds = true;

	DurationOption(float seconds) : seconds(seconds)
	{

	}

	virtual OptionType GetType() override
	{
		return OptionType::Output;
	}

	virtual std::string ToString() override
	{
		return std::string("-t ") + std::to_string(seconds);
	}
};

