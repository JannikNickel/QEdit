#pragma once
#include "Option.h"

struct ResolutionOption : Option
{
	unsigned int width;
	unsigned int height;

	ResolutionOption(unsigned int width, unsigned int height) : width(width), height(height)
	{

	}

	ResolutionOption()
	{
		
	}

	virtual OptionType GetType() override
	{
		return OptionType::Output;
	}

	virtual std::string ToString() override
	{
		return "-s " + std::to_string(width) + "x" + std::to_string(height);
	}

	virtual void ConsoleInput() override
	{
		enabled = ConsoleHelper::YesNoDialog("Change resolution?", false);
		if(enabled)
		{
			std::cout << "Enter width:";
			std::cin >> width;
			std::cout << "Enter height:";
			std::cin >> height;
		}
	}
};
