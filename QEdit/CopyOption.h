#pragma once
#include "Option.h"

/// <summary>
/// Copies all frames of the input file instead of decoding and encoding them
/// </summary>
struct CopyOption : Option
{
	virtual OptionType GetType() override
	{
		return OptionType::Output;
	}

	virtual std::string ToString() override
	{
		return "-c:v copy";
	}
};
