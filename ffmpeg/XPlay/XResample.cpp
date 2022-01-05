#include "XResample.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}
#include <iostream>
#pragma comment(lib, "swresample.lib")
using namespace std;
bool XResample::Open(AVCodecParameters* para, bool isClearPara)
{
    if (!para)return false;

    mux.lock();
    /*if(!actx)
        actx = swr_alloc();*/
    actx = swr_alloc_set_opts(actx,
        av_get_default_channel_layout(2),
        (AVSampleFormat)outFormat,//AV_SAMPLE_FMT_S16
        para->sample_rate,
        av_get_default_channel_layout(para->channels),
        (AVSampleFormat)para->format,
        para->sample_rate,
        0, 0
        );
    if(isClearPara)
        avcodec_parameters_free(&para);
    int re = swr_init(actx);
    mux.unlock();
    if (re != 0)
    {

        cout << "swr_init fail\n";
        return false;
    }
    return true;
}

void XResample::Close()
{
    mux.lock();
    mux.unlock();
}

int XResample::Resample(AVFrame* indata, unsigned char* d)
{
    if (!indata)return 0;
    if (!d)
    {
        av_frame_free(&indata);
        return 0;
    }
    uint8_t* data[2] = { 0 };
    data[0] = d;
    int re = swr_convert(actx, data, indata->nb_samples,
        (const uint8_t**)indata->data, indata->nb_samples);
    if (re <= 0)return re;
    return re*indata->channels*av_get_bytes_per_sample((AVSampleFormat)outFormat);
}

XResample::~XResample()
{
}
XResample::XResample()
{
}
