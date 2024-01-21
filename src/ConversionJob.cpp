#include "pch.h"
#include "ConversionJob.h"

ConversionJob::ConversionJob(VideoHandle* source, CString destination, std::function<void(float, const TCHAR*)> progressCallback)
	: source(source), destination(destination), progressCallback(progressCallback)
{
	isRunning = true;
	worker = std::thread(&ConversionJob::Convert, this);
}

ConversionJob::~ConversionJob()
{
	Stop();
}

void ConversionJob::Stop()
{
	if(isRunning)
	{
		isRunning = false;
		worker.join();
	}
}

void ConversionJob::Convert()
{
	for(size_t i = 0; i < 100; i++)
	{
		progressCallback((i + 1) / 100.0f, nullptr);
		Sleep(100);
	}
}
