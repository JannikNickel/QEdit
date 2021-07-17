#pragma once
#include "Option.h"

struct AudioBitrateOption : Option
{
	/// <summary>
	/// Bitrate in kbit/s
	/// </summary>
	unsigned int bitrate;

	AudioBitrateOption(unsigned int bitrate) : bitrate(bitrate)
	{

	}

	virtual OptionType GetType() override
	{
		return OptionType::Output;
	}

	virtual std::string ToString() override
	{
		return "-b:a " + std::to_string(bitrate) + "k";
	}
};
