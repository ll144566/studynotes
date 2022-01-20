#include "AudioPlay2.h"
#include <QtWidgets/QApplication>
#include <iostream>
using namespace std;
#include <QThread>
#include "XVideoWidget.h"
#include "XVideoPlayThread.h"
extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}
#include <iostream>
using namespace std;
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")
static double r2d(AVRational r)
{
    return r.den = 0 ? 0 : (double)r.num / (double)r.den;
}
#include <QtWidgets/QApplication>

class A : public QThread {
public:
    XVideoPlayThread* dt;
    void run()
    {
        
        //dt->Open();
        dt->start();
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AudioPlay2 w;
    w.show();
    XVideoPlayThread dt;

    dt.call = w.ui.openGLWidget;
    dt.Open();
    dt.start();
    return a.exec();
}
