#pragma once
#include <tuple>
extern "C"
{
	#include "libavformat/avformat.h"
	#include "libavcodec/avcodec.h"
	#include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}

class CQEditDoc : public CDocument
{
public:
	virtual ~CQEditDoc();

	virtual BOOL OnNewDocument();
	std::tuple<int, int> VideoResolution();
	bool HasVideo() const;
	CString* FilePath() const;

	bool SetVideoFile(const char* path);
	bool GetVideoFrame(float time, int width, int height, CBitmap* bitmap) const;

protected:
	CQEditDoc() noexcept;
	DECLARE_DYNCREATE(CQEditDoc)
	DECLARE_MESSAGE_MAP()

private:
	CString* filePath = nullptr;
	AVFormatContext* fContext = nullptr;
	AVStream* vidStream = nullptr;
	AVCodecContext* codecContext = nullptr;

	bool OpenVideoStream(const char* path);
	bool WriteFrameToBitmap(const AVFrame* frame, int width, int height, CBitmap* bitmap) const;
	void FreeResources();
};
