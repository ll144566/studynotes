#include <QtCore/QCoreApplication>
#include <QAudioFormat>
#include <QAudioOutput>

extern "C"
{
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"swresample.lib")
#include <iostream>
#include <qthread.h>
using namespace std;
static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

void play()
{
	QAudioFormat fmt;
	QAudioOutput* output = NULL;
	QIODevice* io = NULL;
	int sampleRate = 44100;
	int sampleSize = 16;
	int channels = 2;
	fmt.setSampleRate(sampleRate);
	fmt.setSampleSize(sampleSize);
	fmt.setChannelCount(channels);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	output = new QAudioOutput(fmt);
	io = output->start();

	char url[] = "v1080.mp4";
	av_register_all();
	avcodec_register_all();
	AVFormatContext* ic = NULL;
	avformat_open_input(&ic, url, NULL, NULL);
	avformat_find_stream_info(ic, 0);
	av_dump_format(ic, 0, url, 0);

	int audioStreamIndex = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	AVStream* audioStream = ic->streams[audioStreamIndex];
	AVCodecParameters* para = audioStream->codecpar;
	AVCodec* codec = avcodec_find_decoder(para->codec_id);
	AVCodecContext* ctx = avcodec_alloc_context3(codec);

	avcodec_parameters_to_context(ctx, para);
	ctx->thread_count = 8;
	int re = avcodec_open2(ctx, 0, 0);


	SwrContext* actx = NULL; //= swr_alloc();
	actx = swr_alloc_set_opts(actx,
		av_get_default_channel_layout(2),
		AV_SAMPLE_FMT_S16,
		para->sample_rate,
		av_get_default_channel_layout(para->channels),
		(AVSampleFormat)para->format,
		para->sample_rate,
		0, 0
	);
	swr_init(actx);

	AVPacket* pkt = av_packet_alloc();
	unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
	while (av_read_frame(ic, pkt) >= 0)
	{
		if (pkt->stream_index != audioStreamIndex)
			continue;

		pkt->pts = pkt->pts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base));
		pkt->dts = pkt->dts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base));
		avcodec_send_packet(ctx, pkt);

		//while (true)
		{
			AVFrame* frame = av_frame_alloc();

			int re = avcodec_receive_frame(ctx, frame);
			//if (re != 0)
				//break;

			uint8_t* data[2] = { 0 };
			data[0] = pcm;
			re = swr_convert(actx, data, frame->nb_samples,
				(const uint8_t**)frame->data, frame->nb_samples);
			int size = re * frame->channels * av_get_bytes_per_sample((AVSampleFormat)1);
			io->write((char*)pcm, size);


		}
	}

	av_packet_free(&pkt);

}
bool play2();
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	

	play2();

    return a.exec();
}



bool play2()
{
	QAudioFormat fmt;
	QAudioOutput* output = NULL;
	QIODevice* io = NULL;
	int sampleRate = 44100;
	int sampleSize = 16;
	int channels = 2;
	fmt.setSampleRate(sampleRate);
	fmt.setSampleSize(sampleSize);
	fmt.setChannelCount(channels);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	output = new QAudioOutput(fmt);
	io = output->start();

	AVCodecContext* codec = 0;
	AVFormatContext* ic = 0;
	int videoStream = 0;
	int audioStream = 1;
	av_register_all();
	avcodec_register_all();
	char url[] = "v1080.mp4";
	int re = avformat_open_input(
		&ic,
		url,
		0,
		NULL
	);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed! :" << buf << endl;
		return false;
	}
	cout << "open " << url << " success! " << endl;

	re = avformat_find_stream_info(ic, 0);

	int totalMs = ic->duration / (AV_TIME_BASE / 1000);
	cout << "totalMs = " << totalMs << endl;

	av_dump_format(ic, 0, url, 0);


	AVStream* as = 0;
	audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	as = ic->streams[audioStream];
	AVCodecParameters* para = as->codecpar;

	AVCodec* vcodec = avcodec_find_decoder(para->codec_id);
	if (!vcodec)
	{
		avcodec_parameters_free(&para);
		cout << "can't find the codec id " << para->codec_id << endl;
		return false;
	}
	cout << "find the AVCodec " << para->codec_id << endl;

	codec = avcodec_alloc_context3(vcodec);

	avcodec_parameters_to_context(codec, para);

	codec->thread_count = 8;

	re = avcodec_open2(codec, 0, 0);
	if (re != 0)
	{
		avcodec_free_context(&codec);
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "avcodec_open2  failed! :" << buf << endl;
		return false;
	}
	SwrContext* actx = 0;
	actx = swr_alloc_set_opts(actx,
		av_get_default_channel_layout(2),
		(AVSampleFormat)1,
		para->sample_rate,
		av_get_default_channel_layout(para->channels),
		(AVSampleFormat)para->format,
		para->sample_rate,
		0, 0
	);
	re = swr_init(actx);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "swr_init  failed! :" << buf << endl;
		return false;
	}
	unsigned char* pcm = new unsigned char[1024 * 1024];
	for (;;)
	{
		AVPacket* pkt = av_packet_alloc();
		int re = av_read_frame(ic, pkt);
		if (re != 0)
		{
			av_packet_free(&pkt);
			return 0;
		}
		//pts×ª»»ÎªºÁÃë
		pkt->pts = pkt->pts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
		pkt->dts = pkt->dts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
		if (pkt->stream_index != audioStream)
			continue;


		if (!codec)
		{
			return false;
		}
		if (!pkt)break;
		re = avcodec_send_packet(codec, pkt);
		av_packet_free(&pkt);

		AVFrame* frame = av_frame_alloc();
		re = avcodec_receive_frame(codec, frame);
		if (re != 0)
		{
			av_frame_free(&frame);
			return NULL;
		}



		uint8_t* data[2] = { 0 };
		data[0] = pcm;
		re = swr_convert(actx,
			data, frame->nb_samples,		//Êä³ö
			(const uint8_t**)frame->data, frame->nb_samples	//ÊäÈë
		);
		if (re <= 0)return re;
		int len = re * frame->channels * av_get_bytes_per_sample((AVSampleFormat)1);



		while (len > 0)
		{
			if (output->bytesFree() >= len)
			{
				io->write((char*)pcm, len);
				break;
			}


		}

	}
	return true;
}
