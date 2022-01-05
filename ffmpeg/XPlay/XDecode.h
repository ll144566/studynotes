#pragma once
#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;

class XDecode
{
public:
	bool isAudio = false;
	//打开解码器, 不管成功与否都要释放空间
	virtual bool Open(AVCodecParameters* para);
	virtual void Close();
	virtual void Clear();
	//当前解码到的pts
	long long pts = 0;
	//发送到解码线程，不管成功与否都要释放pkt空间
	virtual bool Send(AVPacket* pkt);
	//获取解码数据，一次send可能需要多次Recv，获取缓冲的数据Send NULL 再Recv多次
	//每次复制一份，由调用者释放
	virtual AVFrame* Recv();

	XDecode();
	~XDecode();
protected:
	AVCodecContext* codec = 0;
	std::mutex mux;
};

