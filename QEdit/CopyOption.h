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

	OptionType GetType() const override
	{
		return OptionType::Output;
	}

	std::string ToString() const override
	{
		return "-c:v copy";
	}
};
