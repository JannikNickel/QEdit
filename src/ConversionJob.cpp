#include "pch.h"
#include "ConversionJob.h"
extern "C"
{
	#include "libavformat/avformat.h"
	#include "libavcodec/avcodec.h"
	#include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}

ConversionJob::ConversionJob(VideoHandle* source, CString destination, std::function<void(float, const TCHAR*)> progressCallback)
	: source(source), destination(destination), progressCallback(progressCallback)
{
	isRunning = true;
	worker = std::thread(&ConversionJob::Convert, this);
}

ConversionJob::~ConversionJob()
{
	Stop();
}

void ConversionJob::Stop()
{
	if(isRunning)
	{
		isRunning = false;
		worker.join();
	}
}

void ConversionJob::Convert()
{
	progressCallback(0.0f, nullptr);

	std::string outPath = GetTempFilePath().value_or("");
	if(outPath.empty())
	{
		progressCallback(0.0f, _T("Could not generate temporary file!"));
		return;
	}
	outPath = "D://Desktop/test.mp4";

	//Input refs
	AVStream* inputStream = source->Stream();
	AVCodecContext* inputCodecCtx = source->Codec();
	AVFormatContext* inputFmtCtx = source->Format();

	AVFormatContext* outFmtCtx = nullptr;
	AVCodecContext* outCodecCtx = nullptr;
	const AVOutputFormat* outFmt = nullptr;
	const AVCodec* outCodec = nullptr;
	AVStream* outStream = nullptr;
	const AVPixelFormat* pixFmt = nullptr;
	SwsContext* swsCtx = nullptr;
	AVFrame* frame = nullptr;
	AVFrame* scaledFrame = nullptr;
	AVPacket* pkt = nullptr;

	int64_t totalFrames = 0;
	int64_t currFrame = 0;

	const TCHAR* error = nullptr;

	avformat_alloc_output_context2(&outFmtCtx, nullptr, "mp4", outPath.c_str());
	if(outFmtCtx == nullptr)
	{
		error = _T("Could not alloc output format context!");
		goto CLEANUP;
	}
	outFmt = outFmtCtx->oformat;

	outCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if(outCodec == nullptr)
	{
		error = _T("Could not find codec!");
		goto CLEANUP;
	}

	outStream = avformat_new_stream(outFmtCtx, NULL);
	if(outStream == nullptr)
	{
		error = _T("Could not create new stream!");
		goto CLEANUP;
	}

	outCodecCtx = avcodec_alloc_context3(outCodec);
	if(outCodecCtx == nullptr)
	{
		error = _T("Could not alloc output codec context!");
		goto CLEANUP;
	}

	outCodecCtx->codec_id = outCodec->id;
	outCodecCtx->bit_rate = 5000 * 1024;
	outCodecCtx->rc_min_rate = outCodecCtx->bit_rate;
	outCodecCtx->rc_max_rate = outCodecCtx->bit_rate;
	outCodecCtx->bit_rate_tolerance = 0;
	outCodecCtx->rc_buffer_size = outCodecCtx->bit_rate;
	outCodecCtx->width = 1920;
	outCodecCtx->height = 1080;
	outCodecCtx->time_base = AVRational { 1, 60 };
	outCodecCtx->framerate = AVRational { 60, 1 };
	outCodecCtx->gop_size = inputCodecCtx->gop_size;
	outCodecCtx->max_b_frames = inputCodecCtx->max_b_frames;

	//Use pixel format from input stream if possible
	outCodecCtx->pix_fmt = outCodec->pix_fmts[0];
	pixFmt = outCodec->pix_fmts;
	while(*pixFmt != -1)
	{
		if(*pixFmt == inputCodecCtx->pix_fmt)
		{
			outCodecCtx->pix_fmt = *pixFmt;
			break;
		}
		pixFmt++;
	}

	if(avcodec_parameters_from_context(outStream->codecpar, outCodecCtx) < 0)
	{
		error = _T("Failed to get params from context!");
		goto CLEANUP;
	}
	if(avcodec_open2(outCodecCtx, outCodec, nullptr) < 0)
	{
		error = _T("Failed to open codec!");
		goto CLEANUP;
	}

	swsCtx = sws_getContext(inputCodecCtx->width, inputCodecCtx->height, inputCodecCtx->pix_fmt, outCodecCtx->width, outCodecCtx->height, outCodecCtx->pix_fmt, SWS_BILINEAR, nullptr, nullptr, nullptr);
	if(swsCtx == nullptr)
	{
		error = _T("Failed to create scaling context!");
		goto CLEANUP;
	}

	if(avio_open(&outFmtCtx->pb, outPath.c_str(), AVIO_FLAG_WRITE) < 0)
	{
		error = _T("Could not open output file!");
		goto CLEANUP;
	}

	if(avformat_write_header(outFmtCtx, nullptr) < 0)
	{
		error = _T("Could not write file header!");
		goto CLEANUP;
	}

	frame = av_frame_alloc();
	scaledFrame = av_frame_alloc();
	pkt = av_packet_alloc();
	scaledFrame->format = outCodecCtx->pix_fmt;
	scaledFrame->width = outCodecCtx->width;
	scaledFrame->height = outCodecCtx->height;
	if(av_frame_get_buffer(scaledFrame, 0) < 0)
	{
		error = _T("Could not alloc frame buffer!");
		goto CLEANUP;
	}

	totalFrames = inputStream->nb_frames;
	while(av_read_frame(inputFmtCtx, pkt) >= 0 && isRunning)
	{
		if(pkt->stream_index != inputStream->index)
		{
			continue;
		}

		int64_t pts = av_rescale_q_rnd(pkt->pts, inputStream->time_base, outStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		int64_t dts = av_rescale_q_rnd(pkt->dts, inputStream->time_base, outStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		int64_t duration = av_rescale_q(pkt->duration, inputStream->time_base, outStream->time_base);
		pkt->pos = -1;

		if(avcodec_send_packet(inputCodecCtx, pkt) < 0)
		{
			error = _T("Failed to send packet to decoder!");
			goto CLEANUP;
		}

		while(avcodec_receive_frame(inputCodecCtx, frame) == 0)
		{
			sws_scale(swsCtx, frame->data, frame->linesize, 0, inputCodecCtx->height, scaledFrame->data, scaledFrame->linesize);			
			if(avcodec_send_frame(outCodecCtx, scaledFrame) < 0)
			{
				error = _T("Failed to send frame to encoder!");
				goto CLEANUP;
			}
			while(avcodec_receive_packet(outCodecCtx, pkt) == 0)
			{
				pkt->pts = pts;
				pkt->dts = dts;
				pkt->duration = duration;

				if(av_interleaved_write_frame(outFmtCtx, pkt) < 0)
				{
					error = _T("Could not write frame!");
					goto CLEANUP;
				}
			}
		}

		if(++currFrame % 50 == 0)
		{
			progressCallback(static_cast<float>(currFrame / static_cast<double>(totalFrames)), nullptr);
		}
	}

	if(av_write_trailer(outFmtCtx) < 0)
	{
		error = _T("Could not write file trailer!");
		goto CLEANUP;
	}

	CLEANUP:
	av_frame_free(&frame);
	av_frame_free(&scaledFrame);
	av_packet_free(&pkt);
	if(outFmtCtx != nullptr)
	{
		avio_close(outFmtCtx->pb);
	}
	avformat_free_context(outFmtCtx);
	avcodec_close(outCodecCtx);
	avcodec_free_context(&outCodecCtx);
	sws_freeContext(swsCtx);

	progressCallback(1.0f, error);
}

std::optional<std::string> ConversionJob::GetTempFilePath()
{
	char tmpDirPath[MAX_PATH];
	char tmpFilePath[MAX_PATH];
	DWORD len = GetTempPathA(MAX_PATH, tmpDirPath);
	if(len > MAX_PATH)
	{
		return std::nullopt;
	}
	if(GetTempFileNameA(tmpDirPath, "tmp", 0, tmpFilePath) == 0)
	{
		return std::nullopt;
	}
	return std::string(tmpFilePath);
}
