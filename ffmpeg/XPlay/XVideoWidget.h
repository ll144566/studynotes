#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderprogram>
#include <mutex>
#include "IVideoCall.h"
struct AVFrame;
class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions, public IVideoCall
{
	Q_OBJECT

public:
	void Init(int widht, int height);
	//���ܳɹ���񣬶�Ҫ�ͷ�
	virtual void Repaint(AVFrame* frame);
	XVideoWidget(QWidget* parent);
	~XVideoWidget();
protected:
	void paintGL();
	void initializeGL();
	void resizeGL(int width, int height);

private:

	QGLShaderProgram program;
	//shader�л�ȡyuv������ַ
	GLuint unis[3] = { 0 };
	//openg��texure��ַ
	GLuint texs[3] = { 0 };

	//�����ڴ�ռ�
	unsigned char* datas[3] = { 0 };
	int width = 240;
	int height = 128;

	std::mutex mux;


};
