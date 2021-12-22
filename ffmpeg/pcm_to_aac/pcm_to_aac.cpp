extern "C"
{
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include <iostream>
using namespace std;
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swresample.lib")

void Xexit(const char* info)
{
	cout << info << endl;
	exit(1);
}
int main()
{
	char infile[] = "16.wav";
	char outfile[] = "out.aac";
	av_register_all();
	avcodec_register_all();

	//打开音频编码器
	AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (!codec)
		Xexit("avcodec_find_encoder error");

	AVCodecContext* c = avcodec_alloc_context3(codec);
	if (!c)
		Xexit("avcodec_alloc_context3 error");

	c->bit_rate = 64000;
	c->sample_rate = 44100;
	c->sample_fmt = AV_SAMPLE_FMT_FLTP;
	c->channel_layout = AV_CH_LAYOUT_STEREO;
	c->channels = 2;
	c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	int ret = avcodec_open2(c, codec, NULL);
	if (ret != 0)
		Xexit("avcodec_open2 error");

	//2 打开输出封装的上下文
	AVFormatContext* oc = NULL;
	avformat_alloc_output_context2(&oc, NULL, NULL, outfile);
	if (!oc)
	{
		Xexit("avformat_alloc_output_context2 error");
	}
	AVStream* st = avformat_new_stream(oc, NULL);
	st->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(st->codecpar, c);
	av_dump_format(oc, 0, outfile, 1);

	//3 open io, write head
	ret = avio_open(&oc->pb, outfile, AVIO_FLAG_WRITE);
	if (ret != 0)
		Xexit("avio_open error");
	
	ret = avformat_write_header(oc, NULL);

	//4创建音频重采样上下文
	SwrContext* actx = NULL;
	actx = swr_alloc_set_opts(actx,
		c->channel_layout, c->sample_fmt, c->sample_rate, //输出格式
		AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, 44100,//输入格式
		0, 0
	);
	ret = swr_init(actx);
	if (ret != 0)
		Xexit("swr_init error");

	//5 打开输入音频文件，进行重采样
	AVFrame* frame = av_frame_alloc();
	frame->format = AV_SAMPLE_FMT_FLTP;
	frame->channels = 2;
	frame->channel_layout =  AV_CH_LAYOUT_STEREO;
	frame->nb_samples = 1024;//音频单个声道中的采样点数目
	ret = av_frame_get_buffer(frame, 0);
	if (ret != 0)
		Xexit("av_frame_get_buffer error");
	int readSize = frame->nb_samples * 2*2;
	char* pcm = new char[readSize];
	FILE * fp = fopen(infile, "rb");

	for (;;)
	{
		int len = fread(pcm, 1, readSize, fp);
		if (len <= 0)
		{
			break;
		}
		const uint8_t* data[1];
		data[0] = (uint8_t*)pcm;
		len = swr_convert(actx, frame->data, frame->nb_samples, data, frame->nb_samples);
		if (len <= 0)
			break;
		cout << "[" << len << "]";

		//6音频编码
		AVPacket pkt;
		av_init_packet(&pkt);
		ret = avcodec_send_frame(c, frame);
		if (ret < 0)continue;
		ret = avcodec_receive_packet(c, &pkt);

		pkt.stream_index = 0;
		pkt.pts = 0;
		pkt.dts = 0;
		ret = av_interleaved_write_frame(oc, &pkt);


	}

	//写入视频索引
	av_write_trailer(oc);
	//关闭视频输出
	avio_close(oc->pb);
	//清理封装输出上下文
	avformat_free_context(oc);
	//关闭编码器
	avcodec_close(c);
	//清理编码器上下文
	avcodec_free_context(&c);
	//清理视频重采样上下文

}
