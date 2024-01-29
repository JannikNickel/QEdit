#pragma once
#include <string>
#include <functional>

class FFmpegProgressReader
{
public:
	FFmpegProgressReader(double duration, const std::function<void(float)>& progressCallback, const std::function<void()>& errorCallback);

	void StdOut(std::string out);
	void StdErr(std::string err);

private:
	double duration;
	std::function<void(float)> progressCallback;
	std::function<void()> errorCallback;
};
