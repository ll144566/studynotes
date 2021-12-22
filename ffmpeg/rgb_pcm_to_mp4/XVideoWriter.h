#pragma once
#include <iostream>
class AVPacket;
enum XSAMPLEFMT
{
	X_S16 = 1,
	X_FLATP = 8
};
class XVideoWriter
{
public:
	//视频输入参数
	int inWidth = 848;
	int inHeight = 480;
	int inPixFMT = 30; //AV_POX_PMT_BGRA
	
	//音频输入参数
	int inSampleRate = 44100;
	int inChannels = 2;
	XSAMPLEFMT inSampleFmt = X_S16;
	
	//视频输出参数
	int vBtrate = 4000000;
	int outWidth = 848;
	int outheight = 480;
	int outFPS = 25;

	//音频输出参数
	int aBitrate = 6400;
	int outChannels = 2;
	int outSampleRate = 44100;
	XSAMPLEFMT outSampleFmt = X_FLATP;
	int nb_sample = 1024;


	virtual bool Init(const char* file) = 0;
	virtual void Close() = 0;
	virtual bool ADDVideoStream() = 0;
	virtual bool ADDAudioStream() = 0;
	virtual AVPacket *EncodeVideo(const unsigned char* rgb) = 0;
	virtual AVPacket *EncodeAudio(const unsigned char* pcmb) = 0;
	virtual bool WriteHead() = 0;
	virtual bool WriteEnd() = 0;
	virtual bool isVideoBefore() = 0;
	//会释放pkt的空间
	virtual bool WriteFrame(AVPacket *pkt) = 0;
	static XVideoWriter* Get(unsigned short index = 0);
	~XVideoWriter();
	std::string filename;

	
protected:
	XVideoWriter();
};

