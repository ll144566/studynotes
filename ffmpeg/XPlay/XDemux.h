#pragma once
#include <mutex>
class AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
class XDemux
{
public:
	virtual bool Open(const char*url);

	//�ռ���Ҫ�������ͷš� �ͷ�AVPacket����ռ�����ݿռ� av_packet_free
	virtual AVPacket* Read();

	//��ȡ��Ƶ���� ���صĿռ���Ҫ���� avcodec_patameters_free
	virtual AVCodecParameters* CopyVPara();
	//��ȡ��Ƶ���� ���صĿռ���Ҫ����
	virtual AVCodecParameters* CopyAPara();

	//seek λ�� pos 0.0~1.0
	virtual bool Seek(double pos);

	virtual bool IsAudio(AVPacket* pkt);
	//��ն�ȡ����
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

