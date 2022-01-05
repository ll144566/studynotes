#pragma once
#include <mutex>
struct AVCodecParameters;
struct AVFrame;
struct SwrContext;
class XResample
{
public:
	//��������ͳ������һ�³��˲�����ʽ�� ���ΪS16
	virtual bool Open(AVCodecParameters *para, bool isClearPara = false);
	virtual void Close();

	//�����ز����ɹ����Ҫ�ͷ�indata�ռ�
	virtual int Resample(AVFrame *indata, unsigned char* data);
	XResample();
	~XResample();
	int outFormat = 1;


protected:
	std::mutex mux;
	SwrContext* actx = NULL;
};

