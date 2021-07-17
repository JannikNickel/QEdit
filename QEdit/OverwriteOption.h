#pragma once
#include "Option.h"

struct OverwriteOption : Option
{
	bool overwriteFiles = true;

	OverwriteOption(bool overwriteFiles) : overwriteFiles(overwriteFiles)
	{
		
	}

	virtual OptionType GetType() override
	{
		return OptionType::Global;
	}

	virtual std::string ToString() override
	{
		return std::string("-") + (overwriteFiles == true ? "y" : "n");
	}
};

