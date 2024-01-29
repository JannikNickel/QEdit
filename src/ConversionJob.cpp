#include "pch.h"
#include "ConversionJob.h"
#include "FFmpegCli.h"
#include "FFmpegProgressReader.h"
extern "C"
{
	#include "libavformat/avformat.h"
	#include "libavcodec/avcodec.h"
	#include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
	#include "libavutil/opt.h"
}
#include <format>
#include <filesystem>

ConversionJob::ConversionJob(VideoHandle* source, CString destination, const OutputSettings& settings, std::function<void(float, const TCHAR*)> progressCallback)
	: source(source), destination(destination), settings(settings), progressCallback(progressCallback)
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
	std::string outPath = std::string(CW2A(destination));

	#if CONVERT_EXTERN
	ConvertExtern(outPath);
	#else
	ConvertLib(outPath);
	#endif
}

void ConversionJob::ConvertLib(std::string outFile)
{
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
	AVPacket* outPkt = nullptr;
	AVRational inpFps = { 0 };

	int64_t totalFrames = 0;
	int64_t currFrame = 0;

	const TCHAR* error = nullptr;

	if(av_seek_frame(inputFmtCtx, inputStream->index, 0, AVSEEK_FLAG_BACKWARD) < 0)
	{
		error = _T("Could not seek to beginning!");
		goto CLEANUP;
	}

	avformat_alloc_output_context2(&outFmtCtx, nullptr, "mp4", outFile.c_str());
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

	inpFps = av_guess_frame_rate(inputFmtCtx, inputStream, NULL);
	outCodecCtx->width = inputCodecCtx->width;
	outCodecCtx->height = inputCodecCtx->height;
	outCodecCtx->sample_aspect_ratio = inputCodecCtx->sample_aspect_ratio;
	outCodecCtx->bit_rate = 15 * 1000 * 1000;
	outCodecCtx->rc_buffer_size = outCodecCtx->bit_rate * 2;
	outCodecCtx->rc_min_rate = outCodecCtx->bit_rate * 2;
	outCodecCtx->rc_max_rate = outCodecCtx->bit_rate * 2.5;

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

	outCodecCtx->time_base = av_inv_q(inpFps);
	outStream->time_base = outCodecCtx->time_base;

	if(avcodec_open2(outCodecCtx, outCodec, nullptr) < 0)
	{
		error = _T("Failed to open codec!");
		goto CLEANUP;
	}

	if(avcodec_parameters_from_context(outStream->codecpar, outCodecCtx))
	{
		error = _T("Could not set parameters from context");
		goto CLEANUP;
	}

	swsCtx = sws_getContext(inputCodecCtx->width, inputCodecCtx->height, inputCodecCtx->pix_fmt, outCodecCtx->width, outCodecCtx->height, outCodecCtx->pix_fmt, SWS_BILINEAR, nullptr, nullptr, nullptr);
	if(swsCtx == nullptr)
	{
		error = _T("Failed to create scaling context!");
		goto CLEANUP;
	}

	if(avio_open(&outFmtCtx->pb, outFile.c_str(), AVIO_FLAG_WRITE) < 0)
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
	pkt = av_packet_alloc();
	outPkt = av_packet_alloc();
	scaledFrame = av_frame_alloc();
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
		if(pkt->stream_index == inputStream->index)
		{
			if(avcodec_send_packet(inputCodecCtx, pkt) < 0)
			{
				error = _T("Failed to send packet to decoder!");
				goto CLEANUP;
			}

			while(avcodec_receive_frame(inputCodecCtx, frame) == 0)
			{
				sws_scale(swsCtx, frame->data, frame->linesize, 0, inputCodecCtx->height, scaledFrame->data, scaledFrame->linesize);
				av_frame_copy_props(scaledFrame, frame);
				if(avcodec_send_frame(outCodecCtx, scaledFrame) < 0)
				{
					error = _T("Failed to send frame to encoder!");
					goto CLEANUP;
				}
				while(avcodec_receive_packet(outCodecCtx, outPkt) == 0)
				{
					outPkt->stream_index = 0;
					av_packet_rescale_ts(outPkt, inputStream->time_base, outStream->time_base);

					//This can fail, but the video is still fine
					int writeRes = av_interleaved_write_frame(outFmtCtx, outPkt);
				}

				av_frame_unref(frame);
				av_packet_unref(outPkt);
			}

			if(++currFrame % 50 == 0 || currFrame == totalFrames)
			{
				progressCallback(static_cast<float>(currFrame / static_cast<double>(totalFrames)), nullptr);
			}
		}
		av_packet_unref(pkt);
	}

	if(av_write_trailer(outFmtCtx) < 0)
	{
		error = _T("Could not write file trailer!");
		goto CLEANUP;
	}

	CLEANUP:
	av_frame_free(&frame);
	av_frame_unref(scaledFrame);
	av_frame_free(&scaledFrame);
	av_packet_free(&pkt);
	av_packet_free(&outPkt);
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

void ConversionJob::ConvertExtern(std::string outFile)
{
	char fileName[MAX_PATH];
	GetModuleFileNameA(NULL, fileName, MAX_PATH);
	std::filesystem::path ffmpegPath = std::filesystem::path(fileName).parent_path();
	ffmpegPath.append(FFmpegCli::ffmpegPath);
	if(!std::filesystem::exists(ffmpegPath))
	{
		CString err;
		err.Format(_T("Could not find ffmpeg.exe! Expected it at \"%s\""), CString(FFmpegCli::ffmpegPath.c_str()));
		progressCallback(0.0f, err);
	}

	std::string cmd = ffmpegPath.string();
	//Input
	std::string input = std::string(CW2A(*source->Path()));
	cmd += std::format(" -y -i \"{0}\"", input.c_str());
	//Start/end
	if(settings.start != 0.0 && settings.end - settings.start > 0.0)
	{
		cmd += std::format(" -ss {0} -to {1}", settings.start, settings.end);
	}
	//Video
	VideoSettings vSettings = settings.video;
	if(vSettings.resolution.enabled || vSettings.fps.enabled || vSettings.bitrate.enabled || vSettings.codec.enabled)
	{
		if(vSettings.resolution.enabled)
		{
			Resolution res = vSettings.resolution.value;
			cmd += std::format(" -s {0}x{1}", res.width, res.height);
		}
		if(vSettings.fps.enabled)
		{
			cmd += std::format(" -r {0}", vSettings.fps.value);
		}
		if(vSettings.bitrate.enabled)
		{
			Bitrate bitrate = vSettings.bitrate.value;
			cmd += std::format(" -b:v {0}k", bitrate.rate);
			if(bitrate.mode == BitrateMode::CBR)
			{
				cmd += std::format(" -minrate {0}k -maxrate {1}k", bitrate.rate, bitrate.rate);
			}
		}
		if(vSettings.codec.enabled)
		{
			std::string codec = codecNames.find(vSettings.codec.value)->second;
			cmd += std::format(" -c:v {0}", codec);
		}
	}
	else
	{
		cmd += " -c:v copy";
	}
	//Audio
	AudioSettings aSettings = settings.audio;
	if(aSettings.mute || aSettings.bitrate.enabled)
	{
		if(aSettings.mute)
		{
			cmd += " -an";
		}
		else if(aSettings.bitrate.enabled)
		{
			cmd += std::format(" -b:a{0}k", aSettings.bitrate.value);
		}
	}
	else
	{
		cmd += " -c:a copy";
	}
	//Output
	cmd += std::format(" \"{0}\"", outFile);

	bool error = false;
	FFmpegProgressReader reader = FFmpegProgressReader(settings.end - settings.start, [&](float progress)
	{
		progressCallback(progress, NULL);
	}, [&]() { error = true; });
	FFmpegCli cli = FFmpegCli();

	HANDLE handle;
	cli.RunCmd(cmd, std::bind(&FFmpegProgressReader::StdOut, reader, std::placeholders::_1), std::bind(&FFmpegProgressReader::StdErr, reader, std::placeholders::_1), handle);

	progressCallback(1.0f, error ? _T("Error occurred!") : NULL);
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
