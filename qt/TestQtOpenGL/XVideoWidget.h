#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderprogram>
class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	XVideoWidget(QWidget *parent);
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


};
