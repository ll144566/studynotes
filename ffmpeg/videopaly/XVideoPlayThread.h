#pragma once
#include <QThread>
#include <mutex>
#include "XVideoWidget.h"
class AVFormatContext;
struct AVCodecParameters;
struct AVCodecContext;
class XVideoPlayThread :public QThread
{
public:
	//�������󲢴�
	virtual void Open();


	void run();
	XVideoPlayThread();
	virtual ~XVideoPlayThread();
	bool isExit = false;

	std::mutex mux;

	AVFormatContext* ic = NULL;
	//����Ƶ��������ȡʱ��������Ƶ
	int videoStream = 0;
	int totalMs = 0;
	int width = 0;
	int height = 0;
	int sampleRate = 0;
	int channels = 0;
	XVideoWidget* call = 0;
	AVCodecParameters* para = 0;
	AVCodecContext* codec = 0;

};

