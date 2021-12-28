#include <iostream>
#include <windows.h>
#include <filesystem>
#include <string>
#include <vector>
#include "Options.h"
#include "UI.h"
#include "UIErrorDialog.h"
#include "UIProgressDialog.h"

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
	InputFileOption* inOption = new InputFileOption();
	options.push_back(inOption);
	options.push_back(new StartTimeOption());
	options.push_back(new DurationOption());
	options.push_back(new ResolutionOption());
	options.push_back(new FramerateOption());
	options.push_back(new BitrateOption());
	options.push_back(new MuteOption());
	options.push_back(new AudioBitrateOption());
	OutputFileOption* outOption = new OutputFileOption();
	options.push_back(outOption);
	options.push_back(new OverwriteOption());

	UI ui;
	ui.Run(options, [&]
	{
		std::vector<std::string> errors;
		for(Option* op : options)
		{
			std::string error = "";
			bool valid = op->ReadUIValues(error);
			if(valid == false)
			{
				errors.push_back(error);
			}
		}
		if(errors.size() > 0)
		{
			/*ui.ShowDialog(new UIErrorDialog(errors));*/
			ui.ShowDialog(new UIProgressDialog("Encoding"));
			return;
		}

		//Show progress dialog and begin encoding
		std::string ffmpegPath = GetWorkingDirectory() + "\\lib\\ffmpeg.exe";
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
		//TODO if none of the video or audio options are enabled, set the copy video or copy audio flags
		std::system(cmd.c_str());
	});

	//Cleanup
	for(Option* op : options)
	{
		delete(op);
	}
	options.clear();
}
