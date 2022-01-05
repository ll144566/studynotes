#include "XDecode.h"
extern "C"
{
#include "libavcodec/avcodec.h"

}
#include <iostream>
using namespace std;
bool XDecode::Open(AVCodecParameters* para)
{
    if (!para) return false;
    AVCodec* avcodec = avcodec_find_decoder(para->codec_id);
    if (!avcodec)
    {
        avcodec_parameters_free(&para);
        cout << "avcodec_find_decoder fail\n";
        return false;
    }
    mux.lock();
    codec = avcodec_alloc_context3(avcodec);
    avcodec_parameters_to_context(codec, para);
    codec->thread_count = 8;
    int re = avcodec_open2(codec, 0, 0);
    if (re != 0)
    {
        avcodec_parameters_free(&para);
        avcodec_free_context(&codec);
        mux.unlock();
        cout << "avcodec_open2 fail\n";
        return false;
    }
    avcodec_parameters_free(&para);
    mux.unlock();
    return true;
}

void XDecode::Close()
{
    mux.lock();
    if (codec)
    {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    pts = 0;
    mux.unlock();
}

void XDecode::Clear()
{
    mux.lock();
    //ÇåÀí½âÂë»º³å
    if (codec)
        avcodec_flush_buffers(codec);
    mux.unlock();
}

bool XDecode::Send(AVPacket* pkt)
{
    if (!pkt || pkt->size <= 0 || !pkt->data)return false;
    mux.lock();
    if (!codec)
    {
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec, pkt);
    mux.unlock();
    av_packet_free(&pkt);
    if (re != 0)
        return false;
    return true;
}

AVFrame* XDecode::Recv()
{

    mux.lock();
    if (!codec)
    {
        mux.unlock();
        return NULL;
    }
    AVFrame* frame = av_frame_alloc();
    
    int re = avcodec_receive_frame(codec, frame);
    if (re != 0)
    {
        av_frame_free(&frame);
        mux.unlock();
        return NULL;
    }
    mux.unlock();
    pts = frame->pts;
    return frame;
}

XDecode::XDecode()
{
}

XDecode::~XDecode()
{
}
