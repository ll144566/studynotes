#pragma once
#include <mutex>
struct AVCodecParameters;
struct AVFrame;
struct SwrContext;
class XResample
{
public:
	//输出参数和出入参数一致除了采样格式， 输出为S16
	virtual bool Open(AVCodecParameters *para, bool isClearPara = false);
	virtual void Close();

	//不管重采样成功与否都要释放indata空间
	virtual int Resample(AVFrame *indata, unsigned char* data);
	XResample();
	~XResample();
	int outFormat = 1;


protected:
	std::mutex mux;
	SwrContext* actx = NULL;
};

