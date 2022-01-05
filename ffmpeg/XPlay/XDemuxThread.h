#pragma once
#include <mutex>
#include <QThread>
class IVideoCall;

class XDemux;
class XVideoThread;
class XAudioThread;
class XDemuxThread :public QThread
{
public:
	bool Open(const char* url, IVideoCall* call);
	virtual void Start();
	void run();

	XDemuxThread();
	~XDemuxThread();
	bool isExit = false;

protected:
	XDemux* demux = 0;
	XVideoThread* vt = 0;
	XAudioThread* at = 0;
	std::mutex mux;
};

