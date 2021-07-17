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

	AudioBitrateOption()
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

	virtual void ConsoleInput() override
	{
		enabled = ConsoleHelper::YesNoDialog("Change audio bitrate?", false);
		if(enabled)
		{
			std::cout << "Enter bitrate (kbit/s):";
			std::cin >> bitrate;
		}
	}
};
