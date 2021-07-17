#pragma once
#include "Option.h"

struct DurationOption : Option
{
	float seconds = true;

	DurationOption(float seconds) : seconds(seconds)
	{

	}

	DurationOption()
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

	virtual void ConsoleInput() override
	{
		enabled = ConsoleHelper::YesNoDialog("Set duration?", false);
		if(enabled)
		{
			std::cout << "Enter duration:";
			std::cin >> seconds;
		}
	}
};

