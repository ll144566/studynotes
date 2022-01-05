#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>

using namespace std;
bool XDemuxThread::Open(const char* url, IVideoCall* call)
{
	mux.lock();
	if (!demux) demux = new XDemux();
	if (!vt)vt = new XVideoThread();
	if (!at)at = new XAudioThread();
	bool re = demux->Open(url);
	if (!re)
	{
		cout << "demux->Open failed\n";
		return false;
	}
	if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
	{
		re = false;
		cout << "vt->Open failed\n";
	}
	if (!at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels))
	{
		re = false;
		cout << "at->Open failed\n";
	}
	mux.unlock();

	return re;
}

void XDemuxThread::Start()
{
	mux.lock();
	QThread::start();
	if (vt)vt->start();
	if (at)at->start();
	mux.unlock();
}

void XDemuxThread::run()
{
	while (!isExit)
	{
		mux.lock();
		if (!demux)
		{
			mux.unlock();
			msleep(5);
			cout << "sleep 5 s\n";
			continue;
		}
		//ÒôÊÓÆµÍ¬²½
		if (vt && at)
		{
			vt->syncpts = at->pts;
		}
		AVPacket *pkt = demux->Read();
		if (!pkt)
		{
			mux.unlock();
			msleep(1);
			cout << "sleep 1 s\n";
			continue;
		}
		if (demux->IsAudio(pkt))
		{

			if(at)at->Push(pkt);
		}
		else
		{
			if (vt)vt->Push(pkt);
		}
		mux.unlock();
	}
}

XDemuxThread::XDemuxThread()
{
}

XDemuxThread::~XDemuxThread()
{
	isExit = true;
	wait();
}
