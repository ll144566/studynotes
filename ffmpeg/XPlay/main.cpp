#include "XPlay.h"
#include "XDemux.h"
#include "XResample.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "XDecode.h"
#include "XAudioPlay.h"
#include <QThread>
#include "XAudioThread.h"
#include "XVideoThread.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}
using namespace std;
class TestThread :public QThread
{
public:
    XDemux demux;
    XDecode vdecode;
    XDecode adecode;
    XVideoWidget* video;
    XResample resample;
    XAudioThread at;
    XVideoThread vt;
    void Init()
    {
        char path1[] = "rtmp://58.200.131.2:1935/livetv/hunantv";
        char path2[] = "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov";
        char path3[] = "test.mp4";

        cout << demux.Open(path3);




        cout << "vdecode.Open = " << vdecode.Open(demux.CopyVPara()) << endl;
        cout << "adecode.Open = " << adecode.Open(demux.CopyAPara()) << endl;
        cout << "resample.Open() = " << resample.Open(demux.CopyAPara()) << endl;

        at.Open(demux.CopyAPara(), demux.sampleRate, demux.channels);
        at.start();
        vt.Open(demux.CopyVPara(), video, demux.width, demux.height);
        vt.start();
    }
    unsigned char* pcm = new unsigned char[1024 * 1024];
    void run()
    {
        for (;;)
        {
            AVPacket* pkt = demux.Read();
            if (demux.IsAudio(pkt))
            {
                at.Push(pkt);
            }
            else
            {
                vt.Push(pkt);
            }
            if (!pkt)break;

        }

    }
};
extern "C"
{
#include <libavformat/avformat.h>

}
#include "XDemuxThread.h"
int main(int argc, char* argv[])
{




    QApplication a(argc, argv);
    XPlay w;
    w.show();

    //³õÊ¼»¯´°¿Ú
    /*TestThread t;
    
    w.ui.video->Init(t.demux.width, t.demux.height);
    t.video = w.ui.video;
    t.Init();
    t.start();*/

    XDemuxThread dt;
    char path1[] = "rtmp://58.200.131.2:1935/livetv/hunantv";
    char path2[] = "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov";
    char path3[] = "test.mp4";
    char path4[] = "v1080.mp4";
    dt.Open(path3, w.ui.video);
    dt.Start();

   
    return a.exec();
}
