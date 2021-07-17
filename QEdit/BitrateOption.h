#pragma once
#include "Option.h"

struct BitrateOption : Option
{
	/// <summary>
	/// Bitrate in kbit/s
	/// </summary>
	unsigned int bitrate;
	//VBR = -b:v 0k (recommended), -minrate 0k (min), -maxrate 0k (max)

	BitrateOption(unsigned int bitrate) : bitrate(bitrate)
	{

	}

	virtual OptionType GetType() override
	{
		return OptionType::Output;
	}

	virtual std::string ToString() override
	{
		return "-b:v " + std::to_string(bitrate) + "k";
	}
};
