#pragma once
#include "Option.h"

struct OutputFileOption : Option
{
	std::string url;

	OutputFileOption(std::string url) : url(url)
	{

	}

	virtual OptionType GetType() override
	{
		return OptionType::OutputFile;
	}

	virtual std::string ToString() override
	{
		return "\"" + url + "\"";
	}
};
