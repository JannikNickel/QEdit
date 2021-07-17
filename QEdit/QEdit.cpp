#include <iostream>
#include <windows.h>
#include <filesystem>
#include <string>
#include "Options.h"
#include <vector>

std::string GetWorkingDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

int main()
{
	std::vector<Option*> options;
	OptionType optionOrder[] =
	{
		OptionType::Global,
		OptionType::Input,
		OptionType::InputFile,
		OptionType::Output,
		OptionType::OutputFile
	};

	InputFileOption* inOption = new InputFileOption();
	options.push_back(inOption);
	options.push_back(new StartTimeOption());
	options.push_back(new DurationOption());
	options.push_back(new ResolutionOption());
	options.push_back(new FramerateOption());
	options.push_back(new BitrateOption());
	options.push_back(new CopyOption());
	options.push_back(new MuteOption());
	options.push_back(new AudioBitrateOption());
	options.push_back(new AudioCopyOption());
	OutputFileOption* outOption = new OutputFileOption();
	options.push_back(outOption);
	options.push_back(new OverwriteOption());

	for(Option* op : options)
	{
		op->ConsoleInput();
	}
	if(outOption->url.empty())
	{
		outOption->url = inOption->url;
	}

	std::string ffmpegPath = GetWorkingDirectory() + "\\lib\\ffmpeg\\ffmpeg.exe";
	std::string cmd = ffmpegPath;
	for(OptionType type : optionOrder)
	{
		for(Option* op : options)
		{
			if(op != nullptr && op->enabled == true && op->GetType() == type)
			{
				std::string opString = op->ToString();
				if(opString.empty() == false)
				{
					cmd += " " + op->ToString();
				}
			}
		}
	}
	std::cout << cmd;
	std::system(cmd.c_str());

	//Cleanup
	for(Option* op : options)
	{
		delete(op);
	}
	options.clear();
}
