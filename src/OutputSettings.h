#pragma once

enum class BitrateMode
{
	CBR = 0,
	VBR = 1
};

enum class Codec
{
	H264,
	H265_HEVC,
	MPEG4
};

struct Resolution
{
	int32_t width;
	int32_t height;
};

struct VideoSettings
{
	Resolution resolution;
	int32_t fps;
	int32_t bitrate;
	BitrateMode bitrateMode;
	Codec codec;
};

struct AudioSettings
{
	int8_t mute;
	int32_t bitrate;
};

struct OutputSettings
{
	VideoSettings video;
	AudioSettings audio;
};
