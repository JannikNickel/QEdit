#include "pch.h"
#include "FFmpegProgressReader.h"
#include <regex>
#include <cstdio>

FFmpegProgressReader::FFmpegProgressReader(double duration, const std::function<void(float)>& progressCallback, const std::function<void()>& errorCallback)
	: duration(duration), progressCallback(progressCallback), errorCallback(errorCallback)
{

}

void FFmpegProgressReader::StdOut(std::string out)
{
	//FFmpeg uses stderr for output
}

void FFmpegProgressReader::StdErr(std::string err)
{
	static std::regex timeRegex = std::regex("time=([0-9]{2}):([0-9]{2}):([0-9]{2}).([0-9]{2})");

	if(err.find("Error") != std::string::npos)
	{
		errorCallback();
		return;
	}

	if(err.size() != 0)
	{
		std::cmatch match;
		std::regex_search(err.c_str(), match, timeRegex);
		if(match.size() > 0)
		{
			std::string time = match[0];
			int hours, minutes, seconds, milliseconds;
			if(sscanf_s(time.c_str(), "time=%02d:%02d:%02d.%02d", &hours, &minutes, &seconds, &milliseconds) == 4)
			{
				double t = hours * 60 * 60 + minutes * 60 + seconds + milliseconds * 0.001;
				progressCallback(t / duration);
			}
		}
	}
}
