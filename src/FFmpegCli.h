#pragma once
#include <string>
#include <functional>

class FFmpegCli
{
public:
	static inline const std::string ffmpegPath = "ffmpeg.exe";

	bool RunCmd(std::string cmd, std::function<void(std::string)> stdOutReadCallback, std::function<void(std::string)> stdErrReadCallback, HANDLE& processHandle);
};
