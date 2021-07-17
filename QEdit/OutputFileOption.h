#pragma once
#include "Option.h"

struct OutputFileOption : Option
{
	std::string url;

	OutputFileOption(std::string url) : url(url)
	{

	}

	OutputFileOption()
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

	virtual void ConsoleInput() override
	{
		enabled = true;
		std::cout << "Enter output file:";
		std::cin >> url;
	}
};
