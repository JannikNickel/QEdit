#pragma once
#include "Option.h"

struct InputFileOption : Option
{
	std::string url;

	InputFileOption(std::string url) : url(url)
	{

	}

	InputFileOption()
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

	virtual void ConsoleInput() override
	{
		enabled = true;
		std::cout << "Enter input file: ";
		std::cin >> url;
	}
};
