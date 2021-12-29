#include <iostream>
#include <windows.h>
#include <filesystem>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "Options.h"
#include "UI.h"
#include "UIErrorDialog.h"
#include "UIProgressDialog.h"
#include "FFmpegLogReader.h"
#include "CmdExecute.h"

#include <fstream>

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
	inOption->enabled = true;
	options.push_back(inOption);
	StartTimeOption* startOption = new StartTimeOption();
	options.push_back(startOption);
	DurationOption* durationOption = new DurationOption();
	options.push_back(durationOption);
	options.push_back(new ResolutionOption());
	options.push_back(new FramerateOption());
	options.push_back(new BitrateOption());
	options.push_back(new MuteOption());
	options.push_back(new AudioBitrateOption());
	OutputFileOption* outOption = new OutputFileOption();
	outOption->enabled = true;
	options.push_back(outOption);
	options.push_back(new OverwriteOption());
	//TODO option for codec (auto, h265, mpeg4, ...)

	std::string _cmd = R"###("E:\Programmieren\C++\QEdit\Build\Debug\x64\lib\ffmpeg.exe" -i "E:\Programmieren\C++\QEdit\Build\Debug\x64\test.mp4" "E:\Programmieren\C++\QEdit\Build\Debug\x64\out.mp4")###";

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
			ui.ShowDialog(new UIErrorDialog(errors));
			return;
		}

		//Adjust paths
		std::filesystem::path inPath = std::filesystem::path(inOption->url);
		if(inPath.is_relative())
		{
			//inOption->url = std::filesystem::absolute(inPath).string();//Produces "wrong" path when launched from within development environment
			inOption->url = GetWorkingDirectory() + "/" + inOption->url;
		}
		if(outOption->url.empty())
		{
			std::string s = inOption->url;
			int extensionIndex = s.rfind(inPath.extension().string());
			if(extensionIndex != std::string::npos)
			{
				int temp = s.size() - extensionIndex;
				s.erase(extensionIndex, s.size() - extensionIndex);
				s += "_1" + inPath.extension().string();
			}
			outOption->url = s;
		}
		std::filesystem::path outPath = std::filesystem::path(outOption->url);
		if(outPath.is_relative())
		{
			//outOption->url = std::filesystem::absolute(outPath).string();//Produces "wrong" path when launched from within development environment
			outOption->url = GetWorkingDirectory() + "/" + outOption->url;
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

		UIProgressDialog* pDialog = new UIProgressDialog("Encoding");
		ui.ShowDialog(pDialog);

		std::thread t = std::thread([=](UI* ui, UIProgressDialog* pDialog, float startTime, float duration)
		{
			FFmpegLogReader logReader = FFmpegLogReader(startTime, duration);
			//For some reason ffmpeg prints status information to std error
			CmdExecute(_cmd, NULL, [&](std::string data)
			{
				logReader.Read(data);
				float p = logReader.GetProgress();
				pDialog->SetProgress(p);
				ui->ForceRedraw();
			});
		}, &ui, pDialog, startOption->enabled ? startOption->seconds : 0.0f, durationOption->enabled ? durationOption->seconds : 0.0f);
		t.detach();
	});

	//Cleanup
	for(Option* op : options)
	{
		delete(op);
	}
	options.clear();
}
