#pragma once
#include <string>
#include <Windows.h>

static const std::string presetPath = "/presets/";
static const std::string ffmpegPath = "\\lib\\ffmpeg.exe";

static std::string GetWorkingDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}
