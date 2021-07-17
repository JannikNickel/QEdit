#pragma once
#include "Option.h"

struct MuteOption : Option
{
	virtual OptionType GetType() override
	{
		return OptionType::Global;
	}

	virtual std::string ToString() override
	{
		return "-an";
	}

	virtual void ConsoleInput() override
	{
		enabled = ConsoleHelper::YesNoDialog("Mute audio?", false);
	}
};

