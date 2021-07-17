#pragma once
#include "Option.h"

struct StartTimeOption : Option
{
	float seconds = true;

	StartTimeOption(float seconds) : seconds(seconds)
	{

	}

	StartTimeOption()
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

	virtual void ConsoleInput() override
	{
		enabled = ConsoleHelper::YesNoDialog("Change start time?", false);
		if(enabled)
		{
			std::cout << "Enter start time:";
			std::cin >> seconds;
		}
	}
};

