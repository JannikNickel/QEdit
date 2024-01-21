#include "pch.h"
#include "VideoHandle.h"

VideoHandle* VideoHandle::FromFile(const char* path)
{
	AVFormatContext* fContext = NULL;
	int resp = avformat_open_input(&fContext, path, NULL, NULL);
	if(resp < 0)
	{
		avformat_close_input(&fContext);
		return nullptr;
	}

	resp = avformat_find_stream_info(fContext, NULL);
	if(resp < 0)
	{
		avformat_close_input(&fContext);
		return nullptr;
	}

	AVStream* vidStream = nullptr;
	for(size_t i = 0; i < fContext->nb_streams; i++)
	{
		if(fContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			vidStream = fContext->streams[i];
			break;
		}
	}
	if(vidStream == nullptr)
	{
		avformat_close_input(&fContext);
		return nullptr;
	}

	const AVCodec* codec = avcodec_find_decoder(vidStream->codecpar->codec_id);
	if(codec == nullptr)
	{
		avformat_close_input(&fContext);
		return nullptr;
	}

	AVCodecContext* codecContext = avcodec_alloc_context3(codec);
	if(codecContext == nullptr)
	{
		avformat_close_input(&fContext);
		avcodec_free_context(&codecContext);
		return nullptr;
	}

	if(avcodec_parameters_to_context(codecContext, vidStream->codecpar) < 0)
	{
		avformat_close_input(&fContext);
		avcodec_free_context(&codecContext);
		return nullptr;
	}

	if(avcodec_open2(codecContext, codec, NULL) < 0)
	{
		avformat_close_input(&fContext);
		avcodec_free_context(&codecContext);
		return nullptr;
	}

	VideoHandle* handle = new VideoHandle();
	handle->filePath = new CString(path);
	handle->fContext = fContext;
	handle->vidStream = vidStream;
	handle->codecContext = codecContext;
	return handle;
}

VideoHandle::VideoHandle()
{

}

VideoHandle::~VideoHandle()
{
	delete filePath;
	filePath = nullptr;
	avformat_close_input(&fContext);
	avcodec_free_context(&codecContext);
	//vidStream is part of codecContext
}

CString* VideoHandle::Path() const
{
	return filePath;
}

AVFormatContext* VideoHandle::Format() const
{
	return fContext;
}

AVStream* VideoHandle::Stream() const
{
	return vidStream;
}

AVCodecContext* VideoHandle::Codec() const
{
	return codecContext;
}
