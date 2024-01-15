#pragma once
#include <tuple>
#include <vector>
extern "C"
{
	#include "libavformat/avformat.h"
	#include "libavcodec/avcodec.h"
	#include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}

class CQEditDoc : public CDocument
{
	struct FrameData
	{
		int64_t pts;
		AVFrame* frame;
	};

public:
	virtual ~CQEditDoc();

	virtual BOOL OnNewDocument();

	bool HasVideo() const;
	CString* FilePath() const;

	std::tuple<int, int> VideoResolution() const;
	float Duration() const;
	int FrameCount() const;

	bool SetVideoFile(const char* path);
	bool GetVideoFrame(float time, int width, int height, CBitmap* bitmap);

protected:
	CQEditDoc() noexcept;
	DECLARE_DYNCREATE(CQEditDoc)
	DECLARE_MESSAGE_MAP()

private:
	CString* filePath = nullptr;
	AVFormatContext* fContext = nullptr;
	AVStream* vidStream = nullptr;
	AVCodecContext* codecContext = nullptr;
	std::vector<int64_t> iFrames = {};
	std::vector<FrameData> frames = {};
	std::tuple<int64_t, int64_t> loadedFrameRange = { 0, 0 };

	void DiscardLoadedFrames();
	std::tuple<int64_t, int64_t> LoadFrameRange(int64_t lIFrame, int64_t timestamp, bool seek, bool full);
	bool IsInIFrameRange(int64_t lIFrame, int64_t timestamp);

	bool OpenVideoStream(const char* path);
	std::vector<int64_t> FindIFrames();
	bool WriteFrameToBitmap(const AVFrame* frame, int width, int height, CBitmap* bitmap) const;
	void FreeResources();
};
