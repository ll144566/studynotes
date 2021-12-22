#include <iostream>
extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
using namespace std;
void Xexit(const char * info)
{
	cout << info << endl;
	exit(1);
}
int main()
{
	char infile[] = "out.rgb";
	char outfile[] = "rgb.mp4";
	av_register_all();
	avcodec_register_all();

	FILE* fp = fopen(infile, "rb");
	if (!fp)
		Xexit("file open fail");
	int width = 848;
	int height = 480;
	int fps = 25;

	AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!codec)
		Xexit("avcodec_find_encoder fail");

	AVCodecContext* c = avcodec_alloc_context3(codec);
	if (!c)
		Xexit("avcodec_alloc_context3 fail");

	c->bit_rate = 400000000;
	c->width = width;
	c->height = height;
	c->time_base = { 1, fps };
	c->framerate = { fps, 1 };
	c->gop_size = 50;
	c->max_b_frames = 0;
	c->pix_fmt = AV_PIX_FMT_YUV420P;
	c->codec_id = AV_CODEC_ID_H264;

	c->thread_count = 8;
	c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;


	int ret = avcodec_open2(c, codec, NULL);
	if (ret < 0)
		Xexit("avcodec_open2 fail");
	cout << "open success\n";

	AVFormatContext* oc = NULL;
	avformat_alloc_output_context2(&oc, 0, 0, outfile);
	AVStream* st = avformat_new_stream(oc, NULL);
	//st->codec = c;
	st->id = 0;
	st->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(st->codecpar, c);
	cout << "==================================================\n";
	av_dump_format(oc, 0, outfile, 1);
	cout << "==================================================\n";

	//rgb to yuv
	SwsContext* ctx = NULL;
	ctx = sws_getCachedContext(
		ctx,
		width, height, AV_PIX_FMT_RGBA,
		width, height, AV_PIX_FMT_YUV420P,
		SWS_BICUBIC,
		NULL, NULL, NULL
	);

	//输入的空间
	unsigned char* rgb = new unsigned char[width * height * 4];
	//输出的空间
	AVFrame* yuv = av_frame_alloc();
	yuv->format = AV_PIX_FMT_YUV420P;
	yuv->width = width;
	yuv->height = height;
	ret = av_frame_get_buffer(yuv, 32);
	if (ret < 0)
		Xexit("av_frame_get_buffer fail");

	//write mp4 head
	ret = avio_open(&oc->pb, outfile, AVIO_FLAG_WRITE);
	if (ret < 0)
		Xexit("avio_open fail");

	avformat_write_header(oc, NULL);
	int p = 0;
	for (;;)
	{
		int len = fread(rgb, 1, width * height * 4, fp);
		if (len <= 0)break;
		
		uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = rgb;
		int inlinesize[AV_NUM_DATA_POINTERS] = { 0 };
		inlinesize[0] = width * 4;

		int h = sws_scale(ctx,
			indata,
			inlinesize,
			0,
			height,
			yuv->data,
			yuv->linesize);
		if (h <= 0)
			break;
		
		//encode frame
		yuv->pts = p;
		p += 3600;
		yuv->pict_type = AV_PICTURE_TYPE_I;
		ret = avcodec_send_frame(c, yuv);
		if (ret != 0)
		{
			continue;
		}
		
		
		
		AVPacket pkt;
		av_init_packet(&pkt);
		ret = avcodec_receive_packet(c, &pkt);
		if (ret != 0)
			continue;
		
		//av_write_frame(oc, &pkt);
		//av_packet_unref(&pkt);
		av_interleaved_write_frame(oc, &pkt);
		cout << "<" << pkt.size << ">";

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
	sws_freeContext(ctx);
	delete rgb;
	getchar();
	return 0;
}
