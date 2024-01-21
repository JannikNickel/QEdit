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

	//Open and prepare output codec
	AVFormatContext* outFmtCtx = nullptr;
	avformat_alloc_output_context2(&outFmtCtx, nullptr, "mp4", outPath.c_str());
	if(outFmtCtx == nullptr)
	{
		//ERROR
	}
	const AVOutputFormat* outFmt = outFmtCtx->oformat;

	const AVCodec* outCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if(outCodec == nullptr)
	{
		//ERROR
	}

	AVStream* outStream = avformat_new_stream(outFmtCtx, NULL);
	if(outStream == nullptr)
	{
		//ERROR
	}

	AVCodecContext* outCodecCtx = avcodec_alloc_context3(outCodec);
	if(outCodecCtx == nullptr)
	{
		//ERROR
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
	const AVPixelFormat* pixFmt = outCodec->pix_fmts;
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
		//ERROR
	}
	if(avcodec_open2(outCodecCtx, outCodec, nullptr) < 0)
	{
		//ERROR
	}

	//Init scaling
	SwsContext* swsCtx = sws_getContext(inputCodecCtx->width, inputCodecCtx->height, inputCodecCtx->pix_fmt, outCodecCtx->width, outCodecCtx->height, outCodecCtx->pix_fmt, SWS_BILINEAR, nullptr, nullptr, nullptr);
	if(swsCtx == nullptr)
	{
		//ERROR
	}

	//Open file
	if(avio_open(&outFmtCtx->pb, outPath.c_str(), AVIO_FLAG_WRITE) < 0)
	{
		//ERROR
	}

	//Write header
	if(avformat_write_header(outFmtCtx, nullptr) < 0)
	{
		//ERROR
	}

	//Convert and write frames
	AVFrame* frame = av_frame_alloc();
	AVFrame* scaledFrame = av_frame_alloc();
	AVPacket* pkt = av_packet_alloc();
	scaledFrame->format = outCodecCtx->pix_fmt;
	scaledFrame->width = outCodecCtx->width;
	scaledFrame->height = outCodecCtx->height;
	if(av_frame_get_buffer(scaledFrame, 0) < 0)
	{
		//ERROR
	}

	int64_t total = inputStream->nb_frames;
	int64_t fi = 0;
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

		if(avcodec_send_packet(inputCodecCtx, pkt) == 0)
		{
			//ERROR
		}

		while(avcodec_receive_frame(inputCodecCtx, frame) == 0)
		{
			sws_scale(swsCtx, frame->data, frame->linesize, 0, inputCodecCtx->height, scaledFrame->data, scaledFrame->linesize);			
			if(avcodec_send_frame(outCodecCtx, scaledFrame) < 0)
			{
				//ERROR
			}
			while(avcodec_receive_packet(outCodecCtx, pkt) == 0)
			{
				pkt->pts = pts;
				pkt->dts = dts;
				pkt->duration = duration;

				if(av_interleaved_write_frame(outFmtCtx, pkt) < 0)
				{
					//ERROR
				}
			}
		}

		if(++fi % 50 == 0)
		{
			progressCallback(static_cast<float>(fi / static_cast<double>(total)), nullptr);
		}
	}

	bool error = false;
	ERR:


	av_frame_free(&frame);
	av_frame_free(&scaledFrame);
	av_packet_free(&pkt);

	//Write trailer
	av_write_trailer(outFmtCtx);

	//Cleanup
	avio_close(outFmtCtx->pb);
	avcodec_free_context(&outCodecCtx);
	avformat_free_context(outFmtCtx);
	sws_freeContext(swsCtx);

	progressCallback(1.0f, nullptr);
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
