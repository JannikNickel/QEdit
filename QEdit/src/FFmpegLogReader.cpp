#include "FFmpegLogReader.h"
#include <regex>
#include <iostream>

static const std::string durationRegStr = "Duration: ([0-9]{2}):([0-9]{2}):([0-9]{2}).([0-9]{2})";
static const std::regex durationRegex = std::regex(durationRegStr);
static const int durationRegTrim = 10;
static const std::string timeRegStr = "time=([0-9]{2}):([0-9]{2}):([0-9]{2}).([0-9]{2})";
static const std::regex timeRegex = std::regex(timeRegStr);
static const int timeRegTrim = 5;
static const int timeStrLen = 11;

int TryParseInt(std::string str)
{
	int value = 0;
	try
	{
		value = std::stoi(str);
	}
	catch(const std::exception&)
	{

	}
	return value;
}

float ParseTimeStringAsSeconds(std::string str)
{
	int hours = TryParseInt(str.substr(0, 2));
	int minutes = TryParseInt(str.substr(3, 2));
	int seconds = TryParseInt(str.substr(6, 2));
	int milliseconds = TryParseInt(str.substr(9, 2));
	return hours * 60 * 60 + minutes * 60 + seconds + milliseconds * 0.001f;
}

std::string ExtractTimeString(std::string source, std::regex regex, int regTrim)
{
	std::cmatch m;
	std::regex_search(source.c_str(), m, regex);
	if(m.size() == 0)
	{
		return "";
	}
	std::string s = m[0];
	s.erase(0, regTrim);
	return s;
}

void FFmpegLogReader::Read(std::string data)
{
	if(this->targetDuration == 0.0f)
	{
		if(std::regex_search(data, durationRegex))
		{
			std::string durationStr = ExtractTimeString(data, durationRegex, durationRegTrim);

			//Extract numbers (time)
			if(durationStr.length() == timeStrLen)
			{
				float d = ParseTimeStringAsSeconds(durationStr);
				this->targetDuration = d - this->startTime;
				if(this->duration != 0.0f && this->duration < this->targetDuration)
				{
					this->targetDuration = this->duration;
				}
			}
		}
	}
	else
	{
		if(std::regex_search(data, timeRegex))
		{
			std::string timeStr = ExtractTimeString(data, timeRegex, timeRegTrim);

			//Extract numbers (time)
			if(timeStr.length() == timeStrLen)
			{
				this->readDuration = ParseTimeStringAsSeconds(timeStr);
			}
		}
	}
}

float FFmpegLogReader::GetProgress()
{
	if(targetDuration == 0.0f)
	{
		return 0.0f;
	}
	return readDuration / targetDuration;
}
