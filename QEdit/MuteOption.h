#pragma once
#include "Option.h"

struct MuteOption : Option
{
	bool mute = false;

	MuteOption(bool mute) : mute(mute)
	{

	}

	virtual OptionType GetType() override
	{
		return OptionType::Global;
	}

	virtual std::string ToString() override
	{
		return std::string((mute == true ? "-an" : ""));
	}
};

