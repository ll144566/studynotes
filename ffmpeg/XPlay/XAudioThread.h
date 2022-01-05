#pragma once
#include <QThread>
#include <mutex>
#include <list>
struct AVCodecParameters;
struct XDecode;
struct XAudioPlay;
struct XResample;
struct AVPacket;
class XAudioThread:public QThread
{
public:
	//当前音频播放的pts
	long long pts = 0;
	virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);
	virtual void Push(AVPacket* pkt);
	void run();
	XAudioThread();
	virtual ~XAudioThread();

	//最大队列
	int maxList = 100;
	bool isExit = false;
protected:
	std::list <AVPacket*> packs;
	XDecode* decode = 0;
	XAudioPlay* ap = 0;
	XResample* res = 0;
	std::mutex mux;
};

