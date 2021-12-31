#pragma once
#include <string>

enum class OptionType
{
	Global = 0,
	Input = 1,
	InputFile = 2,
	Output = 3,
	OutputFile = 4
};

static const OptionType optionOrder[5] =
{
	OptionType::Global,
	OptionType::Input,
	OptionType::InputFile,
	OptionType::Output,
	OptionType::OutputFile
};
