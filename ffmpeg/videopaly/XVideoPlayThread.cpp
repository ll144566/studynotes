#include "XVideoPlayThread.h"
#include <iostream>
extern "C" {
#include "libavformat/avformat.h"
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
using namespace std;
static double r2d(AVRational r)
{
	return r.den = 0 ? 0 : (double)r.num / (double)r.den;
}
void XVideoPlayThread::run()
{
	//Open();
	while (!isExit)
	{



		//����Ƶͬ��

		AVPacket* pkt = av_packet_alloc();
		//��ȡһ֡��������ռ�
		int re = av_read_frame(ic, pkt);
		if (re != 0)
		{
			av_packet_free(&pkt);
			return;
		}
		//ptsת��Ϊ����
		pkt->pts = pkt->pts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
		pkt->dts = pkt->dts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
		if (!pkt)
		{
			return;
		}
		//�ж���������Ƶ
		if (pkt->stream_index == 1)
		{
		}
		else //��Ƶ
		{
			avcodec_send_packet(codec, pkt);

			//һ��send ���recv
			while (1)
			{
				AVFrame* frame = av_frame_alloc();

				re = avcodec_receive_frame(codec, frame);
				if (re != 0)
				{
					av_frame_free(&frame);
					break;
				}
				if (!frame) break;
				//��ʾ��Ƶ
				if (call)
				{
					msleep(30);
					call->Repaint(frame);
				}

			}
		}


	}
}


void XVideoPlayThread::Open()
{

	av_register_all();
	avcodec_register_all();
	char url[] = "v1080.mp4";
	AVDictionary* opts = NULL;
	int re = avformat_open_input(
		&ic,
		url,
		0,  // 0��ʾ�Զ�ѡ������
		&opts //�������ã�����rtsp����ʱʱ��
	);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed! :" << buf << endl;
		return;
	}
	cout << "open " << url << " success! " << endl;

	re = avformat_find_stream_info(ic, 0);

	int totalMs = ic->duration / (AV_TIME_BASE / 1000);
	cout << "totalMs = " << totalMs << endl;


	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	AVStream* vs = ic->streams[videoStream];
	para = vs->codecpar;
	width = para->width;
	height = para->height;

	if (call)
	{
		call->Init(width, height);
	}

	AVCodec* vcodec = avcodec_find_decoder(para->codec_id);
	cout << "find the AVCodec " << para->codec_id << endl;
	codec = avcodec_alloc_context3(vcodec);
	avcodec_parameters_to_context(codec, para);
	codec->thread_count = 8;
	re = avcodec_open2(codec, vcodec, NULL);
	if (re != 0)
	{
		cout << "avcodec_open2 failed\n";
	}



}

XVideoPlayThread::XVideoPlayThread()
{
}


XVideoPlayThread::~XVideoPlayThread()
{
	isExit = true;
	wait();
}
