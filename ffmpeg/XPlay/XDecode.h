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
	//�򿪽�����, ���ܳɹ����Ҫ�ͷſռ�
	virtual bool Open(AVCodecParameters* para);
	virtual void Close();
	virtual void Clear();
	//��ǰ���뵽��pts
	long long pts = 0;
	//���͵������̣߳����ܳɹ����Ҫ�ͷ�pkt�ռ�
	virtual bool Send(AVPacket* pkt);
	//��ȡ�������ݣ�һ��send������Ҫ���Recv����ȡ���������Send NULL ��Recv���
	//ÿ�θ���һ�ݣ��ɵ������ͷ�
	virtual AVFrame* Recv();

	XDecode();
	~XDecode();
protected:
	AVCodecContext* codec = 0;
	std::mutex mux;
};

