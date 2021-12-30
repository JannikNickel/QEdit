#pragma once
#include "AdditionalOption.h"

/// <summary>
/// Copies all frames of the input file instead of decoding and encoding them
/// </summary>
struct CopyOption : AdditionalOption
{
	CopyOption()
	{
		enabled = true;
	}

	OptionType GetType() override
	{
		return OptionType::Output;
	}

	std::string ToString() override
	{
		return "-c:v copy";
	}
};
