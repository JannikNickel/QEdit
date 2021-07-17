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

	//TODO read options here, show UI or console menu
	options.push_back(new OverwriteOption(true));
	options.push_back(new InputFileOption("D:\\Desktop\\RUST 2021.07.11 - 01.54.44.12.DVR.mp4"));
	options.push_back(new OutputFileOption("D:\\Desktop\\output.mp4"));
	options.push_back(new FramerateOption(60));
	options.push_back(new StartTimeOption(15.5f));
	options.push_back(new DurationOption(10.75f));
	options.push_back(new BitrateOption(7500));
	options.push_back(new MuteOption(true));
	options.push_back(new ResolutionOption(1280, 720));

	std::string ffmpegPath = GetWorkingDirectory() + "\\lib\\ffmpeg\\ffmpeg.exe";
	std::string cmd = ffmpegPath;
	for(OptionType type : optionOrder)
	{
		for(Option* op : options)
		{
			if(op != nullptr && op->GetType() == type)
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
