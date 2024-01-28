#pragma once

template<typename T>
struct Setting
{
	int8_t enabled;
	T value;
};

enum class BitrateMode
{
	CBR = 0,
	VBR = 1
};

enum class Codec
{
	H264 = 0,
	H265_HEVC = 1,
	MPEG4 = 2
};

struct Resolution
{
	uint32_t width;
	uint32_t height;
};

struct Bitrate
{
	uint32_t rate;
	BitrateMode mode;
};

struct VideoSettings
{
	Setting<Resolution> resolution;
	Setting<uint32_t> fps;
	Setting<Bitrate> bitrate;
	Setting<Codec> codec;
};

struct AudioSettings
{
	int8_t mute;
	Setting<uint32_t> bitrate;
};

struct OutputSettings
{
	VideoSettings video;
	AudioSettings audio;
};
