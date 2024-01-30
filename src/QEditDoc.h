#pragma once
#include <afxwin.h>
#include <tuple>
#include <vector>
#include "VideoHandle.h"
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

	DECLARE_DYNCREATE(CQEditDoc)

public:
	~CQEditDoc();

	BOOL OnNewDocument() override;

	bool HasVideo() const;
	VideoHandle* GetVideoHandle() const;
	CString* FilePath() const;
	std::tuple<int, int> VideoResolution() const;
	double Duration() const;
	int FrameCount() const;
	double AvgFps() const;

	bool SetVideoFile(const char* path);
	bool GetVideoFrame(double time, int width, int height, CBitmap* bitmap);

protected:
	VideoHandle* vidHandle = nullptr;
	std::vector<int64_t> iFrames = {};
	std::vector<FrameData> frames = {};
	std::tuple<int64_t, int64_t> loadedFrameRange = { 0, 0 };

	CQEditDoc() noexcept { }

	void DiscardLoadedFrames();
	std::tuple<int64_t, int64_t> LoadFrameRange(int64_t lIFrame, int64_t timestamp, bool seek, bool full);
	bool IsInIFrameRange(int64_t lIFrame, int64_t timestamp);

	bool OpenVideoStream(const char* path);
	std::vector<int64_t> FindIFrames();
	bool WriteFrameToBitmap(const AVFrame* frame, int width, int height, CBitmap* bitmap) const;
	void FreeResources();

	DECLARE_MESSAGE_MAP()
};
