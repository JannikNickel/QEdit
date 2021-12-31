#pragma once
#include "AdditionalOption.h"

/// <summary>
/// Copies all frames of the input file instead of decoding and encoding them
/// </summary>
struct AudioCopyOption : AdditionalOption
{
	AudioCopyOption()
	{
		enabled = true;
	}

	OptionType GetType() const override
	{
		return OptionType::Output;
	}

	std::string ToString() const override
	{
		return "-c:a copy";
	}
};
