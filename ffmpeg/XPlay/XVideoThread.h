#pragma once
#include <QThread>
struct AVPacket;
struct AVCodecParameters;
class XDecode;
class XAudioPlay;
#include <list>
#include <mutex>
#include "IVideoCall.h"
class XVideoThread :public QThread
{
public:
	virtual bool Open(AVCodecParameters* para, IVideoCall *call, int widht, int height);
	virtual void Push(AVPacket* pkt);
    XVideoThread();
    ~XVideoThread();
	void run();
	int maxList = 100;
	bool isExit = false;
	long long syncpts = 0;
protected:
	std::list <AVPacket*> packs;
	XDecode* decode = 0;
	XAudioPlay* ap = 0;
	std::mutex mux;
	IVideoCall *call = 0;
};

