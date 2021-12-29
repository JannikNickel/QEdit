#include <iostream>
#include <windows.h>
#include <filesystem>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include "Options.h"
#include "UI.h"
#include "UIErrorDialog.h"
#include "UIProgressDialog.h"
#include "FFmpegLogReader.h"
#include "CmdExecute.h"

//Status fields
std::atomic<bool> isEncoding = false;
std::thread* encodingThread = nullptr;

//Function declarations
std::string GetWorkingDirectory();
bool ValidateEncodingSettings(OptionCollection options, std::vector<std::string>& errors);
void StartEncoding(UI& ui, OptionCollection options);
void AdjustInOutPaths(std::string& inputPath, std::string& outputPath);
std::string BuildFFmpegCmdLine(OptionCollection options);

int main()
{
	//Create option instances
	OptionCollection options;
	options.push_back(new InputFileOption());
	options.push_back(new StartTimeOption());
	options.push_back(new DurationOption());
	options.push_back(new ResolutionOption());
	options.push_back(new FramerateOption());
	options.push_back(new BitrateOption());
	options.push_back(new MuteOption());
	options.push_back(new AudioBitrateOption());
	options.push_back(new OutputFileOption());
	options.push_back(new OverwriteOption());
	//TODO option for codec (auto, h265, mpeg4, ...)

	std::string _cmd = R"###("E:\Programmieren\C++\QEdit\Build\Debug\x64\lib\ffmpeg.exe" -i "E:\Programmieren\C++\QEdit\Build\Debug\x64\test.mp4" "E:\Programmieren\C++\QEdit\Build\Debug\x64\out.mp4")###";

	//Run UI and define lambda for encode button callback
	UI ui;
	ui.Run(options, [&]
	{
		if(!isEncoding.load())
		{
			//Read UI values and check for errors
			std::vector<std::string> errors;
			if(!ValidateEncodingSettings(options, errors))
			{
				ui.ShowDialog(new UIErrorDialog(errors));
				return;
			}

			//Start encoding process
			StartEncoding(ui, options);
		}
	});

	//Cleanup
	for(Option* op : options)
	{
		delete(op);
	}
	options.clear();
}

std::string GetWorkingDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

bool ValidateEncodingSettings(OptionCollection options, std::vector<std::string>& errors)
{
	errors.clear();
	//Read UI values and validate them
	for(Option* op : options)
	{
		std::string error = "";
		bool valid = op->ReadUIValues(error);
		if(valid == false)
		{
			errors.push_back(error);
		}
	}
	return errors.size() > 0;
}

void StartEncoding(UI& ui, OptionCollection options)
{
	//Set flag to prevent UI from start another encoding
	isEncoding.store(true);

	InputFileOption* inOption = FindOption<InputFileOption>(options);
	OutputFileOption* outOption = FindOption<OutputFileOption>(options);
	StartTimeOption* startOption = FindOption<StartTimeOption>(options);
	DurationOption* durationOption = FindOption<DurationOption>(options);

	//Adjust input/output paths (absolute paths, directories exist, ...)
	AdjustInOutPaths(inOption->url, outOption->url);

	//TODO if none of the video or audio options are enabled, set the copy video or copy audio flags

	//Fenerate ffmpeg cmd line string from options
	std::string cmd = BuildFFmpegCmdLine(options);

	//Show progress dialog on the UI
	UIProgressDialog* pDialog = new UIProgressDialog("Encoding");
	ui.ShowDialog(pDialog);

	//Begin encoding and read log to update the progress on the UI
	encodingThread = new std::thread([=](UI* ui, UIProgressDialog* pDialog, float startTime, float duration)
	{
		FFmpegLogReader logReader = FFmpegLogReader(startTime, duration);
		//For some reason ffmpeg prints status information to std error instead of std out
		CmdExecute(cmd, NULL, [&](std::string data)
		{
			logReader.Read(data);
			float p = logReader.GetProgress();
			pDialog->SetProgress(p);
			ui->ForceRedraw();
		});
		isEncoding.store(false);
	}, &ui, pDialog, startOption->enabled ? startOption->seconds : 0.0f, durationOption->enabled ? durationOption->seconds : 0.0f);
	encodingThread->detach();
}

void AdjustInOutPaths(std::string& inputPath, std::string& outputPath)
{
	//Make input path absolute
	std::filesystem::path inPath = std::filesystem::path(inputPath);
	if(inPath.is_relative())
	{
		//inOption->url = std::filesystem::absolute(inPath).string();//Produces "wrong" path when launched from within development environment
		inputPath = GetWorkingDirectory() + "/" + inputPath;
	}

	//If output path is empty, use input path + _1 appendix
	if(outputPath.empty())
	{
		std::string s = inputPath;
		int extensionIndex = s.rfind(inPath.extension().string());
		if(extensionIndex != std::string::npos)
		{
			int temp = s.size() - extensionIndex;
			s.erase(extensionIndex, s.size() - extensionIndex);
			s += "_1" + inPath.extension().string();
		}
		outputPath = s;
	}

	//Make output path absolute
	std::filesystem::path outPath = std::filesystem::path(outputPath);
	if(outPath.is_relative())
	{
		outputPath = GetWorkingDirectory() + "/" + outputPath;
	}
}

std::string BuildFFmpegCmdLine(OptionCollection options)
{
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
	return cmd;
}
