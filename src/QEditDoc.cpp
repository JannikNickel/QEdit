#include "pch.h"
#include "framework.h"

#include "QEdit.h"
#include "QEditDoc.h"

#include <propkey.h>

IMPLEMENT_DYNCREATE(CQEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CQEditDoc, CDocument)
END_MESSAGE_MAP()

CQEditDoc::CQEditDoc() noexcept
{

}

CQEditDoc::~CQEditDoc()
{

}

BOOL CQEditDoc::OnNewDocument()
{
	if(!CDocument::OnNewDocument())
	{
		return FALSE;
	}
	FreeResources();
	return TRUE;
}

bool CQEditDoc::HasVideo() const
{
	return filePath != nullptr;
}

CString* CQEditDoc::FilePath() const
{
	return filePath;
}

std::tuple<int, int> CQEditDoc::VideoResolution() const
{
	return vidStream != nullptr
		? std::make_tuple(vidStream->codecpar->width, vidStream->codecpar->height)
		: std::make_tuple(0, 0);
}

float CQEditDoc::Duration() const
{
	return vidStream != nullptr ? vidStream->duration * av_q2d(vidStream->time_base) : 0.0f;
}

int CQEditDoc::FrameCount() const
{
	return vidStream != nullptr ? static_cast<int>(av_q2d(vidStream->avg_frame_rate) * Duration()) : 0;
}

bool CQEditDoc::SetVideoFile(const char* path)
{
	if(!OpenVideoStream(path))
	{
		return false;
	}

	UpdateAllViews(NULL);
	return true;
}

bool CQEditDoc::GetVideoFrame(float time, int width, int height, CBitmap* bitmap) const
{
	int64_t timestamp = static_cast<int64_t>(time * AV_TIME_BASE);
	if(av_seek_frame(fContext, vidStream->index, timestamp, AVSEEK_FLAG_BACKWARD) < 0)
	{
		return false;
	}

	AVPacket* packet = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();
	bool found = false;
	while(av_read_frame(fContext, packet) >= 0)
	{
		if(packet->stream_index == vidStream->index)
		{
			if(avcodec_send_packet(codecContext, packet) < 0)
			{
				break;
			}

			int res = 0;
			bool error = false;
			while(res >= 0)
			{
				res = avcodec_receive_frame(codecContext, frame);
				if(res < 0)
				{
					error = res != AVERROR(EAGAIN) && res != AVERROR_EOF;
					break;
				}
				found = true;
				WriteFrameToBitmap(frame, static_cast<int>(round(width / 4.0) * 4.0), static_cast<int>(round(height / 4.0) * 4.0), bitmap);
				break;
			}

			if(found || error)
			{
				break;
			}
		}
	}

	av_packet_free(&packet);
	av_frame_free(&frame);
	return found;
}

bool CQEditDoc::OpenVideoStream(const char* path)
{
	AVFormatContext* fContext = NULL;
	int resp = avformat_open_input(&fContext, path, NULL, NULL);
	if(resp < 0)
	{
		avformat_close_input(&fContext);
		return false;
	}

	resp = avformat_find_stream_info(fContext, NULL);
	if(resp < 0)
	{
		avformat_close_input(&fContext);
		return false;
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
		return false;
	}

	const AVCodec* codec = avcodec_find_decoder(vidStream->codecpar->codec_id);
	if(codec == nullptr)
	{
		avformat_close_input(&fContext);
		return false;
	}

	AVCodecContext* codecContext = avcodec_alloc_context3(codec);
	if(codecContext == nullptr)
	{
		avformat_close_input(&fContext);
		avcodec_free_context(&codecContext);
		return false;
	}

	if(avcodec_parameters_to_context(codecContext, vidStream->codecpar) < 0)
	{
		avformat_close_input(&fContext);
		avcodec_free_context(&codecContext);
		return false;
	}

	if(avcodec_open2(codecContext, codec, NULL) < 0)
	{
		avformat_close_input(&fContext);
		avcodec_free_context(&codecContext);
		return false;
	}

	FreeResources();
	this->filePath = new CString(path);
	this->fContext = fContext;
	this->vidStream = vidStream;
	this->codecContext = codecContext;
	return true;
}

bool CQEditDoc::WriteFrameToBitmap(const AVFrame* frame, int width, int height, CBitmap* bitmap) const
{
	const AVPixelFormat pixFmt = AV_PIX_FMT_BGR24;

	BITMAPINFO info;
	memset(&info, 0, sizeof(BITMAPINFO));
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = width;
	info.bmiHeader.biHeight = height;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = width * height * 3;

	HDC hDC = GetDC(nullptr);
	void* dibData = nullptr;

	HBITMAP hDibBitmap = CreateDIBSection(hDC, &info, DIB_RGB_COLORS, &dibData, nullptr, 0);
	if(hDibBitmap == nullptr || dibData == nullptr)
	{
		ReleaseDC(nullptr, hDC);
		return false;
	}

	AVFrame* bgrFrame = av_frame_alloc();
	int nBytes = av_image_get_buffer_size(pixFmt, width, height, 1);
	uint8_t* buffer = (uint8_t*)av_malloc(nBytes);
	av_image_fill_arrays(bgrFrame->data, bgrFrame->linesize, buffer, pixFmt, width, height, 1);

	SwsContext* swsCtx = sws_getContext(frame->width, frame->height, static_cast<AVPixelFormat>(frame->format), width, height, pixFmt, SWS_BILINEAR, nullptr, nullptr, nullptr);
	sws_scale(swsCtx, frame->data, frame->linesize, 0, frame->height, bgrFrame->data, bgrFrame->linesize);

	memcpy(dibData, bgrFrame->data[0], nBytes);
	bitmap->Attach(hDibBitmap);

	av_frame_free(&bgrFrame);
	av_free(buffer);
	sws_freeContext(swsCtx);
	ReleaseDC(nullptr, hDC);
	return true;
}

void CQEditDoc::FreeResources()
{
	delete filePath;
	filePath = nullptr;
	avformat_close_input(&fContext);
	avcodec_free_context(&codecContext);
	//vidStream is part of codecContext
}
