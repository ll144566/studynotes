#include "XVideoWriter.h"
extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}
#include <iostream>
using namespace std;
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")
class CXVideoWriter :public XVideoWriter
{
public:
	AVFormatContext* ic = NULL; //封装MP4输出上下文
	AVCodecContext* vc = NULL;  //视频编码器上下文
	AVCodecContext* ac = NULL;  //音频编码器上下文
	AVStream* vs = NULL;        //视频流
	AVStream* as = NULL;        //音频流
	SwsContext* vsc = NULL;     //像素转换上下文
	SwrContext* asc = NULL;     //音频重采样上下文
	AVFrame* yuv = NULL;        //输出yuv
	AVFrame* pcm = NULL;        //重采样输出pcm
	int vpts = 0;               //视频的pts
	int apts = 0;               //音频的pts
	void Close()
	{
		if (ic) avformat_close_input(&ic);
		if (vc) 
		{
			avcodec_close(vc);
			avcodec_free_context(&vc);
		}
		if (ac)
		{
			avcodec_close(ac);
			avcodec_free_context(&ac);
		}
		if (vsc)
		{
			sws_freeContext(vsc);
			vsc = NULL;
		}
		if (asc)
		{
			swr_free(&asc);
		}
		if (yuv)
			av_frame_free(&yuv);
		if (pcm)
			av_frame_free(&pcm);
	}
	bool Init(const char* file)
	{
		Close();
		avformat_alloc_output_context2(&ic, NULL, NULL, file);
		if (!ic)
		{
			cerr << "avformat_alloc_output_context2 failed\n";
			return false;
		}
		filename = file;
		return true;
	}
	bool ADDVideoStream()
	{
		if (!ic)return false;
		AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec)
		{
			cerr << "avcodec_find_encoderAV_CODEC_ID_H264 failed\n";
			return false;
		}
		vc = avcodec_alloc_context3(codec);
		if (!vc)
		{
			cerr << "avcodec_alloc_context3 failed\n";
			return false;
		}
		vc->bit_rate = vBtrate;
		vc->width = outWidth;
		vc->height = outheight;

		//时间基础
		vc->time_base = { 1, outFPS };
		vc->framerate = { outFPS, 1 };

		//画面大小 多山帧一个关键帧
		vc->gop_size = 50;

		vc->max_b_frames = 0;

		vc->pix_fmt = AV_PIX_FMT_YUV420P;

		vc->codec_id = AV_CODEC_ID_H264;
		
		av_opt_set(vc->priv_data, "preset", "superfast", 0);
		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		int ret = avcodec_open2(vc, codec, NULL);
		if (ret != 0)
		{
			cerr << "avcodec_open2 failed\n";
			return false;
		}
		cout << "avcodec_open2 success!\n";

		//添加视频流到输出上下文
		vs = avformat_new_stream(ic, NULL);
		vs->codecpar->codec_tag = 0;
		avcodec_parameters_from_context(vs->codecpar, vc);
		av_dump_format(ic, 0, filename.c_str(), 1);

		//像素转换上下文 rgb to yuv
		vsc = sws_getCachedContext(vsc,
			inWidth, inHeight, (AVPixelFormat)inPixFMT,
			outWidth, outheight, AV_PIX_FMT_YUV420P,
			SWS_BICUBIC,
			NULL, NULL, NULL);
		if (!vsc)
		{
			cerr << "sws_getCachedContext failed\n";
			return false;
		}
		if (!yuv)
		{
			yuv = av_frame_alloc();
			yuv->format = AV_PIX_FMT_YUV420P;
			yuv->width = outWidth;
			yuv->height = outheight;
			yuv->pts = 0;
			int ret = av_frame_get_buffer(yuv, 32);
			if (ret != 0)
			{
				cerr << "av_frame_get_buffer failed\n";
				return false;
			}
		}
		return true;

	}

	AVPacket* EncodeVideo(const unsigned char* rgb)
	{
		AVPacket* p = NULL;
		uint8_t* inData[AV_NUM_DATA_POINTERS] = { 0 };
		inData[0] = (uint8_t*)rgb;

		int insize[AV_NUM_DATA_POINTERS] = { 0 };
		insize[0] = inWidth * 4;


		int h = sws_scale(vsc, inData, insize, 0, inHeight,
			yuv->data, yuv->linesize);
		if (h < 0)
			return p;

		yuv->pts = vpts;
		vpts++;
		/*yuv->pts = vpts;
		vpts += 3600;*/

		//encode
		int ret = avcodec_send_frame(vc, yuv);
		if (ret != 0)
			return NULL;
		p = av_packet_alloc();
		ret = avcodec_receive_packet(vc, p);
		if (ret != 0 || p->size == 0)
		{
			av_packet_free( &p);
			return NULL;
		}

		av_packet_rescale_ts(p, vc->time_base, vs->time_base);
  		return p;
	}

	bool WriteHead()
	{
		if (!ic) return false;
		//打开IO
		int ret = avio_open(&ic->pb, filename.c_str(), AVIO_FLAG_WRITE);
		if (ret != 0)
		{
			cerr << "avio_open fail\n";
			return false;
		}
			
		ret = avformat_write_header(ic, NULL);
		if (ret != 0)
		{
			cerr << "avformat_write_header fail\n";
			return false;
		}
		cout << "write " << filename << " head success\n";
		return true;
	}
	bool WriteEnd()
	{
		if (!ic || !ic->pb) return false;
		//打开IO


		
		if (av_write_trailer(ic) != 0)
		{
			cerr << "av_write_trailer fail\n";
			return false;
		}
		
		//关闭输入io
		if (avio_close(ic->pb) != 0)
		{
			cerr << "avio_close fail\n";
			return false;
		}
		cout << "write " << filename << " trailer success\n";
		return true;
	}


	bool WriteFrame(AVPacket *pkt)
	{
		if (!ic || !pkt || pkt->size <= 0) return false;
		av_interleaved_write_frame(ic, pkt);
		return true;
	}

	bool ADDAudioStream()
	{
		if (!ic)return false;
		//1找到音频编码
		AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
		if (!codec)
		{
			cerr << "avcodec_find_encoder fail\n";
			return false;
		}
		ac = avcodec_alloc_context3(codec);
		if(!ac)
		{
			cerr << "avcodec_alloc_context3 fail\n";
			return false;
		}
		ac->bit_rate = aBitrate;
		ac->sample_rate = outSampleRate;
		ac->sample_fmt = (AVSampleFormat)outSampleFmt;
		ac->channels = outChannels;
		ac->channel_layout = av_get_default_channel_layout(outChannels);
		ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		int ret = avcodec_open2(ac, codec, NULL);
		if (ret != 0)
		{
			avcodec_free_context(&ac);
			cerr << "avcodec_open2 failed\n";
			return false;
		}
		cout << "avcodec_open2 AV_CODEC_ID_AAC success \n";

		as = avformat_new_stream(ic, NULL);
		if (!as)
		{
			cerr << "avformat_new_stream failed\n";
			return false;
		}
		avcodec_parameters_from_context(as->codecpar, ac);
		as->codecpar->codec_tag = 0;
		av_dump_format(ic, 0, filename.c_str(), 1);

		asc = swr_alloc_set_opts(asc,
			ac->channel_layout, ac->sample_fmt, ac->sample_rate,//输出格式
			av_get_default_channel_layout(inChannels), (AVSampleFormat)inSampleFmt, inSampleRate,
			0, 0);
		ret = swr_init(asc);
		if (ret != 0)
		{
			cerr << "swr_init failed !\n";
			return false;
		}

		//音频重采样后输出AVFramme
		if (!pcm)
		{
			pcm = av_frame_alloc();
			pcm->format = ac->sample_fmt;
			pcm->channels = ac->channels;
			pcm->channel_layout = ac->channel_layout;
			pcm->nb_samples = nb_sample;
			ret = av_frame_get_buffer(pcm, 0);
			{
				cerr << "av_frame_get_buffer failed !\n";
				return false;
			}
		}
		cout << "audio AVFrame create success!" << endl;

		return true;
	}

	AVPacket* EncodeAudio(const unsigned char* d)
	{
		//1 音频重采样
		const uint8_t* data[AV_NUM_DATA_POINTERS];
		data[0] = (uint8_t*)d;
		int len = swr_convert(asc, pcm->data, pcm->nb_samples,
			data, pcm->nb_samples);
		cout << "[" << len << "]";

		//2音频的编码
		int ret = avcodec_send_frame(ac, pcm);
		if (ret != 0)
		{
			cerr << "avcodec_send_frame failed !\n";
			return NULL;
		}
		AVPacket* pkt = av_packet_alloc();
		av_init_packet(pkt);
		ret = avcodec_receive_packet(ac, pkt);
		if (ret != 0)
		{
			cerr << "avcodec_receive_packet failed !\n";
			return NULL;
		}
		pkt->stream_index = as->index;
		pkt->pts = apts;
		pkt->dts = pkt->pts;
		apts += av_rescale_q(pcm->nb_samples, { 1, ac->sample_rate }, ac->time_base);
		return pkt;
	}

	bool isVideoBefore()
	{
		if (!ic || !as || !vs) return false;
		int re = av_compare_ts(vpts, vc->time_base, apts, ac->time_base);
		if (re <= 0)
			return true;
		return false;
	}
	
};



XVideoWriter* XVideoWriter::Get(unsigned short index)
{
	static bool isfirst = true;
	if (isfirst)
	{
		av_register_all();
		avcodec_register_all();
		isfirst = false;
	}
	static CXVideoWriter wrs[65535];
	return &wrs[index];
}

XVideoWriter::~XVideoWriter()
{
}

XVideoWriter::XVideoWriter()
{
}
