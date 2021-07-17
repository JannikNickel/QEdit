#pragma once
#include "Option.h"

struct InputFileOption : Option
{
	std::string url;

	InputFileOption(std::string url) : url(url)
	{

	}

	virtual OptionType GetType() override
	{
		return OptionType::InputFile;
	}

	virtual std::string ToString() override
	{
		return "-i \"" + url + "\"";
	}
};
