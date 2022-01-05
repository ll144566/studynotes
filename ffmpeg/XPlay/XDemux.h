#pragma once
#include <mutex>
class AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
class XDemux
{
public:
	virtual bool Open(const char*url);

	//空间需要调用者释放。 释放AVPacket对象空间和数据空间 av_packet_free
	virtual AVPacket* Read();

	//获取视频参数 返回的空间需要清理 avcodec_patameters_free
	virtual AVCodecParameters* CopyVPara();
	//获取音频参数 返回的空间需要清理
	virtual AVCodecParameters* CopyAPara();

	//seek 位置 pos 0.0~1.0
	virtual bool Seek(double pos);

	virtual bool IsAudio(AVPacket* pkt);
	//清空读取缓存
	virtual void Clear();
	virtual void Close();
	XDemux();
	~XDemux();

	int width = 0;
	int height = 0;
	int sampleRate = 0;
	int channels = 0;
protected:
	std::mutex mux;
	AVFormatContext* ic = NULL;
	int totalMs = 0;
	int videoStream = 0;
	int audioStream = 1;
};

