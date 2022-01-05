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
	//不管成功与否，都要释放
	virtual void Repaint(AVFrame* frame);
	XVideoWidget(QWidget* parent);
	~XVideoWidget();
protected:
	void paintGL();
	void initializeGL();
	void resizeGL(int width, int height);

private:

	QGLShaderProgram program;
	//shader中获取yuv变量地址
	GLuint unis[3] = { 0 };
	//openg的texure地址
	GLuint texs[3] = { 0 };

	//材质内存空间
	unsigned char* datas[3] = { 0 };
	int width = 240;
	int height = 128;

	std::mutex mux;


};
