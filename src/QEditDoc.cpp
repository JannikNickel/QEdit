#include "pch.h"
#include "framework.h"

#include "QEdit.h"
#include "QEditDoc.h"

IMPLEMENT_DYNCREATE(CQEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CQEditDoc, CDocument)
END_MESSAGE_MAP()

CQEditDoc::~CQEditDoc()
{
	FreeResources();
}

BOOL CQEditDoc::OnNewDocument()
{
	FreeResources();
	return CDocument::OnNewDocument();
}

bool CQEditDoc::HasVideo() const
{
	return vidHandle != nullptr;
}

VideoHandle* CQEditDoc::GetVideoHandle() const
{
	return vidHandle;
}

CString* CQEditDoc::FilePath() const
{
	return HasVideo() ? vidHandle->Path() : nullptr;
}

std::tuple<int, int> CQEditDoc::VideoResolution() const
{
	return HasVideo()
		? std::make_tuple(vidHandle->Stream()->codecpar->width, vidHandle->Stream()->codecpar->height)
		: std::make_tuple(0, 0);
}

double CQEditDoc::Duration() const
{
	return HasVideo() ? vidHandle->Stream()->duration * av_q2d(vidHandle->Stream()->time_base) : 0.0;
}

int CQEditDoc::FrameCount() const
{
	return HasVideo() ? static_cast<int>(av_q2d(vidHandle->Stream()->avg_frame_rate) * Duration()) : 0;
}

double CQEditDoc::AvgFps() const
{
	return HasVideo() ? av_q2d(vidHandle->Stream()->avg_frame_rate) : 0.0;
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

bool CQEditDoc::GetVideoFrame(double time, int width, int height, CBitmap* bitmap)
{
	int64_t timestamp = static_cast<int64_t>(time / av_q2d(vidHandle->Stream()->time_base));
	auto [loadedMin, loadedMax] = loadedFrameRange;
	if(frames.size() == 0 || (loadedMin > timestamp || timestamp > loadedMax))
	{
		bool isInRange = IsInIFrameRange(loadedMin, timestamp);
		if(!isInRange)
		{
			DiscardLoadedFrames();
		}
		loadedFrameRange = LoadFrameRange(loadedMin, timestamp, !isInRange, false);
	}

	for(size_t i = 0; i < frames.size(); i++)
	{
		if(frames[i].pts >= timestamp)
		{
			size_t closestFrameIndex = i > 0 && abs(frames[i - 1].pts - timestamp) < abs(frames[i].pts - timestamp) ? i - 1 : i;
			return WriteFrameToBitmap(frames[closestFrameIndex].frame, static_cast<int>(round(width / 4.0) * 4.0), static_cast<int>(round(height / 4.0) * 4.0), bitmap);
		}
	}
	return false;
}

void CQEditDoc::DiscardLoadedFrames()
{
	for(FrameData frame : frames)
	{
		av_frame_unref(frame.frame);
		av_frame_free(&frame.frame);
	}
	frames.clear();
}

std::tuple<int64_t, int64_t> CQEditDoc::LoadFrameRange(int64_t lIFrame, int64_t timestamp, bool seek, bool full)
{
	if(seek && av_seek_frame(vidHandle->Format(), vidHandle->Stream()->index, timestamp, AVSEEK_FLAG_BACKWARD) < 0)
	{
		return { 0, 0 };
	}

	AVPacket* packet = av_packet_alloc();
	bool found = false;
	bool firstPkt = true;
	bool firstFrm = true;
	int64_t start = !seek ? lIFrame : INT64_MAX;
	int64_t end = 0;
	while(av_read_frame(vidHandle->Format(), packet) >= 0)
	{
		if(packet->stream_index == vidHandle->Stream()->index)
		{
			if(!firstPkt && full && packet->flags & AV_PKT_FLAG_KEY)
			{
				break;
			}
			firstPkt = false;

			if(avcodec_send_packet(vidHandle->Codec(), packet) < 0)
			{
				break;
			}

			int res = 0;
			bool error = false;
			AVFrame* frame = av_frame_alloc();
			while(res >= 0)
			{
				res = avcodec_receive_frame(vidHandle->Codec(), frame);
				if(res < 0)
				{
					error = res != AVERROR(EAGAIN) && res != AVERROR_EOF;
					break;
				}

				start = min(start, frame->pts);
				end = frame->pts;
				found = !full && frame->pts >= timestamp;
				frames.push_back(FrameData { .pts = frame->pts, .frame = frame });
				break;
			}

			if(found || error)
			{
				break;
			}
		}
		av_packet_unref(packet);
	}

	av_packet_free(&packet);
	return { start, end };
}

bool CQEditDoc::IsInIFrameRange(int64_t lIFrame, int64_t timestamp)
{
	for(int i = iFrames.size() - 1; i >= 0; i--)
	{
		if(iFrames[i] < timestamp)
		{
			return iFrames[i] == lIFrame;
		}
	}
	return false;
}

bool CQEditDoc::OpenVideoStream(const char* path)
{
	FreeResources();
	this->vidHandle = VideoHandle::FromFile(path);
	if(this->vidHandle == nullptr)
	{
		return false;
	}
	this->iFrames = FindIFrames();
	return true;
}

std::vector<int64_t> CQEditDoc::FindIFrames()
{
	if(av_seek_frame(vidHandle->Format(), vidHandle->Stream()->index, 0, AVSEEK_FLAG_BACKWARD) < 0)
	{
		return { 0, 0 };
	}

	std::vector<int64_t> frames = {};
	AVPacket* packet = av_packet_alloc();
	while(av_read_frame(vidHandle->Format(), packet) >= 0)
	{
		if(packet->stream_index == vidHandle->Stream()->index)
		{
			if(packet->flags & AV_PKT_FLAG_KEY)
			{
				frames.push_back(packet->pts);
			}
		}
	}

	av_packet_free(&packet);
	return frames;
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
	delete vidHandle;
	DiscardLoadedFrames();
}
