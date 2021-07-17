#pragma once
#include "Option.h"

struct ResolutionOption : Option
{
	unsigned int width;
	unsigned int height;

	ResolutionOption(unsigned int width, unsigned int height) : width(width), height(height)
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
};
