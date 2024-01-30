#pragma once
#define CONVERT_EXTERN true
#include "VideoHandle.h"
#include "OutputSettings.h"
#include <functional>
#include <thread>
#include <string>
#include <optional>
#include <map>

class ConversionJob
{
	static inline const std::map<Codec, std::string> codecNames =
	{
		{ Codec::H264, "h264_mf" },
		{ Codec::H265_HEVC, "hevc_nvenc" },
		{ Codec::MPEG4, "mpeg4" },
	};

public:
	ConversionJob(VideoHandle* source, CString destination, const OutputSettings& settings, std::function<void(float progress, const TCHAR* error)> progressCallback);
	~ConversionJob();

	void Stop();

protected:
	VideoHandle* source;
	CString destination;
	OutputSettings settings;
	std::function<void(float, const TCHAR*)> progressCallback;

	bool isRunning;
	std::thread worker;

	void Convert();
	void ConvertLib(std::string outFile);
	void ConvertExtern(std::string outFile);
	std::optional<std::string> GetTempFilePath();
};
