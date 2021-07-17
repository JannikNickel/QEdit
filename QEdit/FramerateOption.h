#pragma once
#include "Option.h"

struct FramerateOption : Option
{
	unsigned int fps;

	FramerateOption(unsigned int fps) : fps(fps)
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
};
