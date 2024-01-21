#pragma once
extern "C"
{
	#include "libavformat/avformat.h"
	#include "libavcodec/avcodec.h"
}

class VideoHandle
{
public:
	static VideoHandle* FromFile(const char* path);

	~VideoHandle();

	CString* Path() const;
	AVFormatContext* Format() const;
	AVStream* Stream() const;
	AVCodecContext* Codec() const;

private:
	VideoHandle();

	CString* filePath = nullptr;
	AVFormatContext* fContext = nullptr;
	AVStream* vidStream = nullptr;
	AVCodecContext* codecContext = nullptr;
};
