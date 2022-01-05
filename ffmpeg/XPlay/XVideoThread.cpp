#include "XVideoThread.h"
#include "XDecode.h"
#include <iostream>
using namespace std;
void XVideoThread::Push(AVPacket* pkt)
{
    if (!pkt)return;
    while (!isExit)
    {
        mux.lock();
        if (packs.size() < maxList)
        {
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
        mux.unlock();
        msleep(1);
    }
}

bool XVideoThread::Open(AVCodecParameters* para, IVideoCall* call, int widht, int height)
{
    
    if (!para)return false;
    syncpts = 0;
    mux.lock();
    this->call = call;
    if (call)
        call->Init(widht, height);
    if (!decode) decode = new XDecode();

    bool re = true;
    
    if (!decode->Open(para))
    {
        cout << "audio Xdecode open failed\n";
        re = false;
    }


    mux.unlock();
    cout << "XAudioThread::Open " << re << endl;;
    return re;
}

void XVideoThread::run()
{
    while (!isExit)
    {
        mux.lock();
        if (packs.empty() || !decode)
        {
            mux.unlock();
            msleep(1);
            continue;
        }

        //ÒôÊÓÆµÍ¬²½
        //printf("syncpts = %d  decode->pts = %d\n", syncpts, decode->pts);

        AVPacket* pkt = packs.front();
        packs.pop_front();
        bool re = decode->Send(pkt);
        if (!re)
        {
            mux.unlock();
            msleep(1);
            continue;
        }
        while (!isExit)
        {
            AVFrame* frame = decode->Recv();
            if (!frame)break;
            if (call)
            {
                msleep(31);
                call->Repaint(frame);
            }

            
        }
        mux.unlock();

    }
}

XVideoThread::XVideoThread()
{
}

XVideoThread::~XVideoThread()
{
    isExit = true;
    wait();
}
