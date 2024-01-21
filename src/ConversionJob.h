#pragma once
#include "VideoHandle.h"
#include <functional>
#include <thread>
#include <string>
#include <optional>

class ConversionJob
{
public:
	ConversionJob(VideoHandle* source, CString destination, std::function<void(float progress, const TCHAR* error)> progressCallback);
	~ConversionJob();
	void Stop();

protected:
	VideoHandle* source;
	CString destination;
	std::function<void(float, const TCHAR*)> progressCallback;

	bool isRunning;
	std::thread worker;

	void Convert();
	std::optional<std::string> GetTempFilePath();
};
