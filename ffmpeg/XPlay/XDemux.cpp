#include "XDemux.h"
#include <iostream>
using namespace std;
extern "C"
{
#include "libavformat/avformat.h"
}
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
static double r2d(AVRational r)
{
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}
bool XDemux::Open(const char* url)
{
    Close();
    mux.lock();
    AVDictionary* opts = NULL;
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    av_dict_set(&opts, "max_delay", "500", 0);
    int re = avformat_open_input(
        &ic,
        url,
        0,
        &opts
    );
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "open " << url << "failed! " << buf << endl;
        return false;
    }
    cout << "open " << url << " success ! " << endl;
    re = avformat_find_stream_info(ic, 0);
    totalMs = ic->duration / (AV_TIME_BASE / 1000);
    cout << "totalMs = " << totalMs << endl;
    av_dump_format(ic, 0, url, 0);
    cout << "==================================================\n";
    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream *vs = ic->streams[videoStream];
    width = vs->codecpar->width;
    height = vs->codecpar->height;

    cout << "codec_id = " << vs->codecpar->codec_id << endl;
    cout << "format = " << vs->codecpar->format << endl;
    cout << "width=" << vs->codecpar->width << endl;
    cout << "height=" << vs->codecpar->height << endl;
    //帧率 fps 分数转换
    cout << "video fps = " << r2d(vs->avg_frame_rate) << endl;
    

    cout << "==================================================\n";

    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    AVStream* as = ic->streams[audioStream];
    sampleRate = as->codecpar->sample_rate;
    channels = as->codecpar->channels;
    cout << "codec_id = " << as->codecpar->codec_id << endl;
    cout << "format = " << as->codecpar->format << endl;
    cout << "sample_rate = " << as->codecpar->sample_rate << endl;
    //AVSampleFormat;
    cout << "channels = " << as->codecpar->channels << endl;
    //一帧数据？？ 单通道样本数 
    cout << "frame_size = " << as->codecpar->frame_size << endl;
 
    mux.unlock();

    return true;
}

AVPacket* XDemux::Read()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return nullptr;
    }
    AVPacket* pkt = av_packet_alloc();
    int re = av_read_frame(ic, pkt);
    if (re != 0)
    {
        mux.unlock();
        av_packet_free(&pkt);
        return nullptr;
    }
    pkt->pts = pkt->pts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base));
    pkt->dts = pkt->dts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base));
    mux.unlock();
    cout << "pkt->pts = " << pkt->pts << "\t" << flush;
    return pkt;
}

AVCodecParameters* XDemux::CopyVPara()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return nullptr;
    }
    AVCodecParameters* pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
    mux.unlock();
    return pa;
}

AVCodecParameters* XDemux::CopyAPara()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return nullptr;
    }
    AVCodecParameters* pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
    mux.unlock();
    return pa;
}

bool XDemux::Seek(double pos)
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return false;
    }
    //清理读取缓冲
    avformat_flush(ic);
    long long seekPos = 0;
    seekPos = ic->streams[videoStream]->duration * pos;
    int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();
    return re >= 0;
}

bool XDemux::IsAudio(AVPacket* pkt)
{
    if (!pkt)return false;
    return pkt->stream_index == audioStream;
}

void XDemux::Clear()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }
    avformat_flush(ic);
    mux.unlock();
}

void XDemux::Close()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }
    avformat_close_input(&ic);
    totalMs = 0;
    mux.unlock();
}

XDemux::XDemux()
{
    static bool isFirst = true;
    static std::mutex dmux;
    dmux.lock();
    if (isFirst)
    {
        av_register_all();
        avformat_network_init();
        isFirst = false;
    }
    dmux.unlock();
}

XDemux::~XDemux()
{
}
