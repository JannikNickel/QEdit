#pragma once
#include "Option.h"

/// <summary>
/// Copies all frames of the input file instead of decoding and encoding them
/// </summary>
struct CopyOption : Option
{
	OptionType GetType() override
	{
		return OptionType::Output;
	}

	std::string ToString() override
	{
		return "-c:v copy";
	}
};
