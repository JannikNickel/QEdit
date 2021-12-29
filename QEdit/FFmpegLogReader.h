#pragma once
#include <string>

class FFmpegLogReader
{
	float readDuration = 0.0f;//Duration from the current log
	float targetDuration = 0.0f;//Duration of the finished video

	float startTime;
	float duration;

public:
	FFmpegLogReader(float startTime, float duration) : startTime(startTime), duration(duration) { }
	void Read(std::string data);
	float GetProgress();
};

