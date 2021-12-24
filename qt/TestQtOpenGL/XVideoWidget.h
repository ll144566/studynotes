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
	//shader�л�ȡyuv������ַ
	GLuint unis[3] = { 0 };
	//openg��texure��ַ
	GLuint texs[3] = { 0 };

	//�����ڴ�ռ�
	unsigned char* datas[3] = { 0 };
	int width = 240;
	int height = 128;


};
