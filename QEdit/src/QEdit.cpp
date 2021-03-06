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
#include "UITextDialog.h"
#include "FFmpegLogReader.h"
#include "CmdExecute.h"
#include "CFGWriter.h"
#include "Settings.h"

//Status fields
std::atomic<bool> isEncoding = false;
std::thread* encodingThread = nullptr;
HANDLE encodingProcess = nullptr;

//Function declarations
bool ValidateEncodingSettings(OptionCollection& options, std::vector<std::string>& errors);
bool ValidateInOutPaths(std::string& in, std::string& out, std::string& error);
bool ValidateOverwriteSettings(std::string& const out, OverwriteOption& const option);
void AddAdditionalOptions(OptionCollection& options);
void StartEncoding(UI& ui, OptionCollection& options);
void AdjustInOutPaths(std::string& inputPath, std::string& outputPath);
std::string BuildFFmpegCmdLine(OptionCollection& const options);
void EncodingFinished(UI& ui, OptionCollection& options, bool success);
BOOL WINAPI ConsoleCtrlHandler(DWORD type);

void TrySavePreset(UI& ui, OptionCollection& options, std::string& const name);
void TryLoadPreset(UI& ui, OptionCollection& options, std::string& const path);

int main()
{
	//Detect console window close
	SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

	//Create option instances
	OptionCollection options;
	options.push_back(new InputFileOption());
	options.push_back(new StartTimeOption());
	options.push_back(new DurationOption());
	options.push_back(new ResolutionOption());
	options.push_back(new FramerateOption());
	options.push_back(new BitrateOption());
	options.push_back(new CodecOption());
	options.push_back(new MuteOption());
	options.push_back(new AudioBitrateOption());
	options.push_back(new OutputFileOption());
	options.push_back(new OverwriteOption());
	

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

			//Adjust input/output paths (absolute paths, directories exist, ...)
			InputFileOption* inOption = FindOption<InputFileOption>(options);
			OutputFileOption* outOption = FindOption<OutputFileOption>(options);
			AdjustInOutPaths(inOption->url, outOption->url);

			//Validate input file exists + output dir exists
			std::string inOutPathError = "";
			bool validPaths = ValidateInOutPaths(inOption->url, outOption->url, inOutPathError);
			if(!validPaths)
			{
				ui.ShowDialog(new UIErrorDialog(std::vector<std::string> { inOutPathError }));
				return;
			}

			//Make sure output file doesnt exist or overwrite flag is set
			if(!ValidateOverwriteSettings(outOption->url, *FindOption<OverwriteOption>(options)))
			{
				ui.ShowDialog(new UIErrorDialog(std::vector<std::string> { "Output file already exists! Please set overwrite flag or change output file name..." }));
				return;
			}

			//Start encoding process
			StartEncoding(ui, options);
		}
	}, [&](std::string presetPath)
	{
		TryLoadPreset(ui, options, presetPath);
	}, [&](std::string presetName)
	{
		TrySavePreset(ui, options, presetName);
	});

	//Cleanup
	for(Option* op : options)
	{
		if(op != nullptr)
		{
			delete op;
		}
	}
	options.clear();
}

bool ValidateEncodingSettings(OptionCollection& options, std::vector<std::string>& errors)
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
	return errors.size() == 0;
}

bool ValidateInOutPaths(std::string& in, std::string& out, std::string& error)
{
	std::filesystem::path inPath = std::filesystem::path(in);
	if(!std::filesystem::exists(inPath))
	{
		error = "Input file doesnt exist!";
		return false;
	}
	std::filesystem::path outPath = std::filesystem::path(out);
	std::filesystem::path outDir = outPath.remove_filename();
	if(!std::filesystem::exists(outDir))
	{
		error = "Output directory doesnt exist!";
		return false;
	}
	return true;
}

bool ValidateOverwriteSettings(std::string& const out, OverwriteOption& const option)
{
	std::filesystem::path outPath = std::filesystem::path(out);
	if(std::filesystem::exists(outPath) && !option.enabled)
	{
		return false;
	}
	return true;
}

void AddAdditionalOptions(OptionCollection& options)
{
	//Add video copy option if no video options are enabled
	BitrateOption* vBitrateOption = FindOption<BitrateOption>(options);
	FramerateOption* vFramerateOption = FindOption<FramerateOption>(options);
	ResolutionOption* vResolutionOption = FindOption<ResolutionOption>(options);
	CodecOption* vCodecOption = FindOption<CodecOption>(options);
	if(!vBitrateOption->enabled && !vFramerateOption->enabled && !vResolutionOption->enabled && !vCodecOption->enabled)
	{
		options.push_back(new CopyOption());
	}

	MuteOption* muteOption = FindOption<MuteOption>(options);
	AudioBitrateOption* aBitrateOption = FindOption<AudioBitrateOption>(options);
	if(!muteOption->enabled && !aBitrateOption->enabled)
	{
		options.push_back(new AudioCopyOption());
	}
}

void StartEncoding(UI& ui, OptionCollection& options)
{
	//Set flag to prevent UI from start another encoding
	isEncoding.store(true);

	StartTimeOption* startOption = FindOption<StartTimeOption>(options);
	DurationOption* durationOption = FindOption<DurationOption>(options);

	//Some options can be added based on the other options settings (for instance audio copy flags if no audio options are enabled)
	AddAdditionalOptions(options);

	//Fenerate ffmpeg cmd line string from options
	std::string cmd = BuildFFmpegCmdLine(options);

	//Show progress dialog on the UI
	UIProgressDialog* pDialog = new UIProgressDialog("Encoding");
	ui.ShowDialog(pDialog);

	//Begin encoding and read log to update the progress on the UI
	encodingProcess = NULL;
	encodingThread = new std::thread([=](std::string cmd, OptionCollection options, UI* ui, UIProgressDialog* pDialog, float startTime, float duration)
	{
		FFmpegLogReader logReader = FFmpegLogReader(startTime, duration);
		//For some reason ffmpeg prints status information to std error instead of std out
		bool success = CmdExecute(cmd, NULL, [&](std::string data)
		{
			logReader.Read(data);
			float p = logReader.GetProgress();
			
			uiDialogMutex.lock();
			if(pDialog != nullptr)
			{
				pDialog->SetProgress(p);
			}
			uiDialogMutex.unlock();

			ui->ForceRedraw();
		}, encodingProcess);
		isEncoding.store(false);
		delete encodingThread;
		EncodingFinished(*ui, options, success);
	}, cmd, options, &ui, pDialog, startOption->enabled ? startOption->seconds : 0.0f, durationOption->enabled ? durationOption->seconds : 0.0f);
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

std::string BuildFFmpegCmdLine(OptionCollection& const options)
{
	std::string cmd = GetWorkingDirectory() + ffmpegPath;
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

void EncodingFinished(UI& ui, OptionCollection& options, bool success)
{
	//Remove additional options and reset all others
	OptionCollection::iterator i = options.begin();
	while(i != options.end())
	{
		Option* op = *i;
		if(IsAdditionalOption(op))
		{
			delete op;
			i = options.erase(i);
			continue;
		}
		op->Reset();
		i++;
	}

	ui.ShowDialog(new UITextDialog("Encoding " + std::string(success ? "finished" : "failed!"), "Encoding process ended...", 1.0f));
	ui.ForceRedraw();
}

BOOL WINAPI ConsoleCtrlHandler(DWORD type)
{
	switch(type)
	{
		case CTRL_CLOSE_EVENT:
		{
			DWORD exitCode = 0;
			if(encodingThread != nullptr)
			{
				delete encodingThread;
				if(encodingProcess && GetExitCodeProcess(encodingProcess, &exitCode))
				{
					if(exitCode == STILL_ACTIVE)
					{
						TerminateProcess(encodingProcess, EXIT_FAILURE);
					}
				}
			}
			break;
		}
		default:
			break;
	}
	return FALSE;
}

void TrySavePreset(UI& ui, OptionCollection& options, std::string& const name)
{
	std::filesystem::path p = GetWorkingDirectory() + presetPath + name + ".cfg";

	try
	{
		std::filesystem::path dirPath = p;
		dirPath.remove_filename();
		if(!std::filesystem::exists(dirPath))
		{
			std::filesystem::create_directory(dirPath);
		}
	}
	catch(const std::exception&) { }
	
	std::ofstream file = std::ofstream(p);
	if(file.fail() || name.empty())
	{
		file.close();
		ui.ShowDialog(new UIErrorDialog({ "Invalid preset name!" }));
		return;
	}

	CFGWriter writer = CFGWriter(&file);
	std::string error;
	for(Option* op : options)
	{
		op->ReadUIValues(error);
		op->SavePreset(writer);
	}

	ui.ShowDialog(new UITextDialog(std::string("Saved preset!"), std::string(""), 1.0f));
}

void TryLoadPreset(UI& ui, OptionCollection& options, std::string& const path)
{
	ui.ShowMainLayer();

	std::ifstream file = std::ifstream(path);
	if(file.fail())
	{
		file.close();
		ui.ShowDialog(new UIErrorDialog({ "Invalid preset file!" }));
		return;
	}

	CFGReader reader = CFGReader(&file);
	std::string error;
	for(Option* op : options)
	{
		op->Reset();
		op->LoadPreset(reader);
	}

	ui.ShowDialog(new UITextDialog(std::string("Loaded preset!"), std::string(""), 1.0f));
}
