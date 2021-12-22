[FFmpeg:AVPacket结构体分析](https://blog.csdn.net/qq_25333681/article/details/89743621)

## AVPacket

在AVPacket结构体的说明部分：有这么一段描述，足够说明它的作用和重要性。

> 该结构存储压缩数据。 它通常由解复用器导出，然后作为输入传递给解码器，或作为编码器的输出接收，然后传递给复用器。
>
> 对于视频而言，它通常应包含一个压缩帧。 对于音频，它可能包含几个压缩帧。 允许编码器输出空包，没有压缩数据，只包含不重要的附加信息数据。例如在编码结束时更新一些流参数。
>
> AVPacket是FFmpeg中为数不多的结构之一，其大小是公共ABI的一部分。 因此，它可以在栈上分配，并且不会添加新的字段，除非libavcodec和libavformat有大的改动。

虽然前面引用部分已经说了`AVPacket`的作用时机，但这里还是单独强调和总结一下：

- 解码时：媒体源数据解封装(解复用)后，解码前。
- 编码时：YUV数据编码后，封装前。

`AVPacket`主要保存一些媒体流的基本信息，例如PTS、DTS时间。最重要的当然就是媒体数据的buffer地址了。

比较重要的有：

- pts：控制显示的pts时间
- dts：控制解码的dts时间
- *data：媒体数据buffer的指针
- duration：AVStream-> time_base单位中此数据包的持续时间，如果未知则为0。 在演示顺序中等于next_pts - this_pts。

### 初始化与清理

~~~c
AVPacket* avPacket = av_packet_alloc(); // 初始化
av_packet_unref(avPacket); // 清理
~~~

没什么稀奇，都是调用特定的函数来处理。这里需要提一下，虽然在第一小节提到，AVPacket是可以在栈内存上分配，也就是这样：AVPacket avPacket，但如非必要，我还是不建议这么做，毕竟有现成的API，就别给自己挖坑了。

还有一个函数：

`int av_packet_ref (AVPacket *dst, const AVPacket *src)`

它的作用是：

> 设置对给定数据包描述的数据的新引用。
>
> 如果src的data指针不为空，则将指针地址作为dst中data指针的值。 否则在dst中分配一个新缓冲区并将数据从src复制到其中。
>
> 所有其他字段都是从src复制的。

这个函数和av_packet_unref函数的作用刚好相反。我们详细了解一下这背后的故事：

我们在一开始有提到AVPacket保存了数据buffer的地址，所以，它实际上并没有包含数据内存本身，只是在它的字段中，用了一个uint8_t *data;来保存数据buffer的地址。

av_packet_ref函数的作用，就是从已有的AVPacket中复制一份出来。关于data部分，如果src的数据是引用计数的，直接把地址拷贝一份，然后把对应buffer的引用计数+1。如果不是，需要新分配一份保存数据的内存空间，并把src中的数据拷贝过来。

而av_packet_unref函数，则会看buffer的引用计数器，如果不为0就-1，为零的话则会清楚掉buffer，AVPacket的其它数据也会回到初始状态。

### 结构定义及成员解读

~~~c
typedef struct AVPacket {
    AVBufferRef *buf; // data的buffer引用指针计数结构体
    int64_t pts; // 控制显示的pts时间
    int64_t dts; // 控制解码的dts时间
    uint8_t *data; // 媒体数据buffer的指针
    int   size; // 数据大小
    int   stream_index; // 流index
    int   flags; // AV_PKT_FLAG值的组合
    AVPacketSideData *side_data; // 容器可以提供的附加数据包数据。 数据包可以包含几种类型的辅助信息。
		// AVStream-> time_base单位中此数据包的持续时间，如果未知则为0。 在演示顺序中等于next_pts  -  this_pts。
    int64_t duration;
    int64_t pos; // 流中的字节位置，如果未知则为-1
} AVPacket;

~~~



~~~c
typedef struct AVBufferRef {
    AVBuffer *buffer;
    // 当且仅当这是对缓冲区的唯一引用时才被认为是可写的，在这种情况下av_buffer_is_writable（）返回1。
    uint8_t *data;
    int      size; // 数据大小（以字节为单位）
} AVBufferRef;

~~~

av_read_frame(ic, pkt);

av_packet_unref(pkt);



## AVFrame

AVFrame 的用法：

1. AVFrame 对象必须调用 av_frame_alloc() 在堆上分配，注意此处指的是 AVFrame 对象本身，AVFrame 对象必须调用 av_frame_free() 进行销毁。
2. AVFrame 中包含的数据缓冲区是
3. AVFrame 通常只需分配一次，然后可以多次重用，每次重用前应调用 av_frame_unref() 将 frame 复位到原始的干净可用的状态。



AVPacket: 存储解码前数据(编码数据:H264/AAC等)
AVFrame: 存储解码后数据(像素数据:YUV/RGB/PCM等)



### av_frame_get_buffer

~~~c
int av_frame_get_buffer(AVFrame *frame, int align);
Required buffer size alignment.  If equal to 0, alignment will be chosen automatically for the current CPU.  It is highly recommended to pass 0 here unless you know what you are doing.
~~~

### [音频处理基本概念及音频重采样](https://www.cnblogs.com/jiayayao/p/8724663.html)

### sws_scale

[FFmpeg: FFmepg中的sws_scale() 函数分析](https://www.cnblogs.com/yongdaimi/p/10715830.html)

# [FFmpeg: FFmepg中的sws_scale() 函数分析](https://www.cnblogs.com/yongdaimi/p/10715830.html)

FFmpeg中的 sws_scale() 函数主要是用来做视频像素格式和分辨率的转换，其优势在于：可以在同一个函数里实现：1.图像色彩空间转换， 2:分辨率缩放，3:前后图像滤波处理。不足之处在于：效率相对较低，不如libyuv或shader，







mp4转yuv

ffmpeg -i test.mp4 test.yuv

播放yuv

ffplay.exe -f rawvideo -video_size 1280x720 -i test.yuv

**MP4 转 RGBA:**

```
ffmpeg -i v1080.mp4 -s 800x400 -pix_fmt rgba 1.rgb
```

其中800x400 代表转换完成后的尺寸，x 是 小写英文字母 x, 不是乘号。

**MP4 转 YUV**

```
ffmpeg -i v1080.mp4 -s 600x300 2.yuv
```

 

yuv420 到 rgb24 格式之间的转换

ffmpeg -s 360x270 -pix_fmt yuv420p -i  2_test_360x270_50.yuv -pix_fmt rgb24  aaaa.rgb

~~~c
ffmpeg -i test.mp4 -pix_fmt bgra test.rgb
ffmpeg -i test.mp4 -f s16le test.pcm
~~~







[avformat_find_stream_info](https://blog.csdn.net/H514434485/article/details/77802382?locationNum=8&fps=1)

调用了avformat_find_stream_info获取到的音视频流信息更完整



# 音频帧概念详解

常人听觉的频率范围大约在20Hz~20kHz之间。

采样频率是指将模拟声音波形进行数字化时，每秒钟抽取声波幅度样本的次数。

根据奈奎斯特采样理论，为了保证声音不失真，采样频率应该在40kHz左右。

常用的音频采样频率有8kHz、11.025kHz、22.05kHz、16kHz、37.8kHz、44.1kHz、48kHz等，

如果采用更高的采样频率，还可以达到DVD的音质。

对采样率为44.1kHz的AAC音频进行解码时，一帧的解码时间须控制在23.22毫秒内。

通常是按1024个采样点一帧

分析：

\1. AAC

一个AAC原始帧包含某段时间内1024个采样点相关数据。

用1024主要是因为AAC是用的1024点的mdct。

音频帧的播放时间 = 一个AAC帧对应的采样样本的个数 / 采样频率(单位为s)。

采样率(samplerate)为 44100Hz，表示每秒 44100个采样点, 

所以，根据公式,  

音频帧的播放时长 = 一个AAC帧对应的采样点个数 / 采样频率

则，当前一帧的播放时间 = 1024 * 1000/44100= 22.32ms(单位为ms)

48kHz采样率，

则，当前一帧的播放时间 = 1024 * 1000000/48000= 21.32ms(单位为ms)

22.05kHz采样率，

则，当前一帧的播放时间 = 1024 * 1000000/22050= 46.43ms(单位为ms)

2.MP3 

mp3 每帧均为1152个字节， 

则：

每帧播放时长 = 1152 * 1000000 / sample_rate

例如：sample_rate = 44100HZ时， 

计算出的时长为26.122ms，

这就是经常听到的mp3每帧播放时间固定为26ms的由来。





YCbCr

Cb：反映的是RGB输入信号蓝色部分与RGB信号亮度值之间的差异。

Cr：反映了RGB输入信号红色部分与RGB信号亮度值之间的差异。



~~~c

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
static double r2d(AVRational r) {
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}
int main(int argc, char* argv[])
{
	av_register_all();
	char path[] = "v1080.mp4";
	AVFormatContext* ac = NULL;
	int re = avformat_open_input(&ac, path, 0, 0);

	if (re != 0) {//打开文件失败
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf));
		printf("file %s open failed because of : %s", path, buf);
		getchar();
		return -1;
	}

	int totalSec = ac->duration / AV_TIME_BASE;
	printf("指定的视频文件有 %d分%d秒\n", totalSec / 60, totalSec % 60);

	int videoStream = 0;
	AVCodecContext* videoCtx = NULL;
	SwsContext* cCtx = NULL;
	int outWidth = 640;
	int outHeight = 480;
	char* rgb = new char[outWidth * outHeight * 4];

	for (int i = 0; i < ac->nb_streams; i++)
	{
		AVCodecContext* enc = ac->streams[i]->codec;

		if (enc->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			videoCtx = enc;
			AVCodec* codec = avcodec_find_decoder(enc->codec_id);
			if (!codec) {
				printf("无法解码此视频文件\n");
				return -1;
			}
			int err = avcodec_open2(enc, codec, NULL);
			if (err != 0) {
				char buf[1024] = { 0 };
				av_strerror(err, buf, sizeof(buf));
				printf(buf);
				return -2;
			}
			printf("\n");
			printf("成功打开视频编码流\n");
		}
	}
	AVFrame* yuv = av_frame_alloc();
	for (;;) {
		AVPacket pkt;
		re = av_read_frame(ac, &pkt);
		if (re != 0) {
			break;
		}
		if (pkt.stream_index != videoStream)
		{
			av_packet_unref(&pkt);
			continue;
		}
		int pts = pkt.pts * r2d(ac->streams[pkt.stream_index]->time_base) * 1000;//得到帧的毫秒值


		int re = avcodec_send_packet(videoCtx, &pkt);
		if (re != 0) {
			av_packet_unref(&pkt);
			continue;
		}
		re = avcodec_receive_frame(videoCtx, yuv);
		if (re != 0) {
			av_packet_unref(&pkt);
			continue;
		}
		printf("[D]___");

		//打开ffmpeg格式转换和缩放器
		cCtx = sws_getCachedContext(cCtx, videoCtx->width, videoCtx->height, videoCtx->pix_fmt,
			outWidth, outHeight, AV_PIX_FMT_BGRA, SWS_BICUBIC,
			NULL, NULL, NULL);
		if (!cCtx) {
			printf("sws_getCachedContext failed!\n");
			break;
		}
		uint8_t* data[AV_NUM_DATA_POINTERS] = { 0 };
		data[0] = (uint8_t*)rgb;
		int linesize[AV_NUM_DATA_POINTERS] = { 0 };
		linesize[0] = outWidth * 4;
		int h = sws_scale(cCtx, yuv->data, yuv->linesize, 0, videoCtx->height, data, linesize);
		if (h > 0) {
			printf("(%d)", h);
		}
		printf("pts = %d 毫秒\n", pts);
		av_packet_unref(&pkt);
	}

	if (cCtx) {
		sws_freeContext(cCtx);
		cCtx = NULL;
	}

	avformat_close_input(&ac);
	ac = NULL;

	return 0;
}
