#pragma once
#include "Option.h"

struct FramerateOption : Option
{
	unsigned int fps;

	FramerateOption(unsigned int fps) : fps(fps)
	{

	}

	FramerateOption()
	{
		
	}

	virtual OptionType GetType() override
	{
		return OptionType::Output;
	}

	virtual std::string ToString() override
	{
		return "-r " + std::to_string(fps);
	}

	virtual void ConsoleInput() override
	{
		enabled = ConsoleHelper::YesNoDialog("Change framerate?", false);
		if(enabled)
		{
			std::cout << "Enter fps:";
			std::cin >> fps;
		}
	}
};
