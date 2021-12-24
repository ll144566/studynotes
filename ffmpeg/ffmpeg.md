# 结构体

## AVFormatContext

关闭一个AVFormatContext，和avformat_open_input()成对使用。声明位于libavformat\avformat.h

**avformat_find_stream_info()**

经过avformat_open_input()调用后已经解析一部分的码流编码信息，但可能不完整不正确。需要进一步调用此函数读取一部分视音频数据并且获得更加完善的编码信息，特别是对于一些无文件头的输入是必要的。

## AVCodecContext

### 变量

```c
max_b_frames  
    /**
     * maximum number of B-frames between non-B-frames
     * Note: The output will be delayed by max_b_frames+1 relative to the input.
     * - encoding: Set by user.
     * - decoding: unused
     */
    
```

从注释中可以看出它指的是两个非B帧之间的B帧的最大数目。

### 函数



## AVPacket

[FFmpeg:AVPacket结构体分析](https://blog.csdn.net/qq_25333681/article/details/89743621)

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

### 判断AVFrame是否为关键帧

通过key_frame判断是否为关键帧。或者 enum AVPictureType pict_type;也行。

### av_frame_get_buffer

~~~c
int av_frame_get_buffer(AVFrame *frame, int align);
Required buffer size alignment.  If equal to 0, alignment will be chosen automatically for the current CPU.  It is highly recommended to pass 0 here unless you know what you are doing.
~~~

### [音频处理基本概念及音频重采样](https://www.cnblogs.com/jiayayao/p/8724663.html)



# 函数

## sws_scale

[FFmpeg: FFmepg中的sws_scale() 函数分析](https://www.cnblogs.com/yongdaimi/p/10715830.html)

## [FFmpeg: FFmepg中的sws_scale() 函数分析](https://www.cnblogs.com/yongdaimi/p/10715830.html)

FFmpeg中的 sws_scale() 函数主要是用来做视频像素格式和分辨率的转换，其优势在于：可以在同一个函数里实现：1.图像色彩空间转换， 2:分辨率缩放，3:前后图像滤波处理。不足之处在于：效率相对较低，不如libyuv或shader，



## AVFrame

**1、av_frame_alloc()：申请AVFrame结构体空间，同时会对申请的结构体初始化。注意哦，这个函数只是创建AVFrame结构的空间，AVFrame中的uint8_t *data[AV_NUM_DATA_POINTERS]空间此时NULL，不会创建的。**

**2、av_frame_free():释放AVFrame的结构体空间。这个函数就有点意思了。因为他不仅仅释放结构体空间，还涉及到AVFrame中的uint8_t *data[AV_NUM_DATA_POINTERS];字段的释放问题。，如果AVFrame中的uint8_t *data[AV_NUM_DATA_POINTERS]中的引用==1，则释放data的空间。**

**3、int av_frame_ref(AVFrame *dst, const AVFrame *src):对已有AVFrame的引用，这个引用做了两个动作：1、将src属性内容复制到dst，2、对AVFrame中的uint8_t *data[AV_NUM_DATA_POINTERS]字段引用计数+1。**

**4、void av_frame_unref(AVFrame *frame);对frame释放引用，做了两个动作：1、将frame的各个属性初始化，2、如果AVFrame中的uint8_t *data[AV_NUM_DATA_POINTERS]中的引用==1，则释放data的空间。当然，如果data的引用计数>1则由别的frame去检测释放。**

**5、av_frame_get_buffer()：这个函数是建立AVFrame中的uint8_t *data[AV_NUM_DATA_POINTERS]内存空间，使用这个函数之前frame结构中的format、width、height：必须赋值，要不然函数怎么知道创建多少字节的空间呢！**



# ffmpeg 命令

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
    
ffmpeg.exe -i v1080.mp4 -t 5 -s 240x128 -pix_fmt yuv420p out240x128.yuv
    //-t 时长 -s尺寸 -pix_fmt格式
~~~



<font color = "red">Renamed Qt GUI Application wizard to Qt Widgets Application (QTVSADDINBUG-589)</font>



[avformat_find_stream_info](https://blog.csdn.net/H514434485/article/details/77802382?locationNum=8&fps=1)

调用了avformat_find_stream_info获取到的音视频流信息更完整

# 音频

## 音频帧概念详解

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

##  关于音频的一些基础知识

[FFmpeg学习3：播放音频](https://www.cnblogs.com/wangguchangqing/p/5788805.html)

和视频一样，音频数据也会被打包到一个容器中，其文件的扩展名并不是其编码的方法，只是其打包文件的格式。
现实世界中，我们所听到的声音是一个个连续的波形，但是计算机无法存储和处理这种拥有无限点的波形数据。所以通过**重采样**，按照一定的
频率（1秒采集多少个点），将有无限个点的连续波形数据转换为有有限个点的离散数据，这就是通常说的A/D转换（模数转换，将模拟数据转换为数字数据）。
通过上面转换过程的描述可以知道，一个数字音频通常由以下三个部分组成：

- **采样频率** 采样是在拥有无限个点的连续波形上选取有限个离散点，采集到的离散点越多，也就越能真实的波形。由于声音是在时间上的连续波形，其采样点的间隔就是两次采样的时间间隔。通俗来说，采样率指的是**每秒钟采样的点数**，单位为HZ。采样率的倒数是采样周期，也就是两次采样的时间间隔。采样率越大，则采集到的样本点数就越多，采样得到的数字音频也就更接近真实的声音波形，相应的其占用的存储空间也就越大。常用的采样频率有：
  - 22k Hz 无限广播所用的采样率
  - 44.1k Hz CD音质
  - 48k Hz 数字电视，DVD
  - 96k Hz 192k Hz 蓝光盘，高清DVD
- **采样精度** 采集到的点被称为样本（sample），每个样本占用的位数就是采样精度。这点和图像的像素表示比较类似，可以使用8bit，16bit或者24bit来表示采集到的一个样本。同样，一个样本占用的空间越大其表示的就越接近真实的声音。
- **通道** 支持不同发声的音响的个数。常用的声道有单声道、双声道、4声道、5.1声道等。不同的声道在采样的时候是不同的，例如双声道，在每次采样的时候有采集两个样本点。

- 比特率

   

  指的是每秒传送的比特（bit）数，其单位是bps（Bit Per Second），是间接衡量声音质量的一个标准。

  - 没有压缩编码的音频数据，其比特率 = 采样频率 * 采样精度 * 通道数，通过该公式可以看出，比特率越高，采样得到的声音质量就越高，相应的占用的存储空间也就越大。
  - 经过压缩编码后的音频数据也有一个比特率，这时候的比特率也可以称之为**码率**，因为其反映了压缩编码的效率。码率越高，压缩后的数据越大，得到音频质量越好，相应的压缩的效率也就越低。
    码率 = 音频文件的大小 / 时长，在时长一定的情况下，码率越高则音频文件越大，其音频的品质也就越高。常见的一些码率：
    - 96 Kbps FM质量
    - 128 - 160 Kbps 比较好的音频质量
    - 192Kbps CD质量
    - 256Kbps 320Kbps 高质量音频

通常我们所说的比特率（码率）指的是编码后的每秒钟的数据量。码率越高，压缩比就越小，音频文件就越大，相对的音频质量也就越好。码率可以反映出音频的质量，码率越高，音频质量就越，反之亦然。





视频播放一般需经过以下几个步骤：解协议，解封装，解码视音频，视音频同步。

- **解协议：**将流媒体协议的数据，解析为标准的相应的封装格式数据。视音频在网络上传播的时候，常常采用各种流媒体协议，例如RTSP，RTMP等等。这些协议在传输视音频数据的同时，也会传输一些信令数据。这些信令数据包括对播放的控制（播放，暂停，停止），或者对网络状态的描述等。解协议过程中会去除掉信令数据而只保留视音频数据。例如，采用RTMP协议传输的数据，经过解协议操作后，输出FLV格式的数据。
- **解封装：**将输入的封装格式的数据，分离成为音频流压缩编码数据和视频流压缩编码数据。封装格式种类很多，例如MP4，MKV，RMVB，TS，FLV，AVI等等，它的作用就是将已经压缩编码的视频数据和音频数据按照一定的格式放到一起。例如，FLV格式的数据，经过解封装操作后，输出H.264编码的视频码流和AAC编码的音频码流。
- **解码**：将视频/音频压缩编码数据，解码成为非压缩的视频/音频原始数据。音频的压缩编码标准包含AAC，MP3，AC-3等等，视频的压缩编码标准则包含H.264，MPEG2，VC-1等等。解码是整个系统中最重要也是最复杂的一个环节。通过解码，压缩编码的视频数据输出成为非压缩的颜色数据，例如YUV420P，RGB等等；压缩编码的音频数据输出成为非压缩的音频抽样数据，例如PCM数据。
- **视音频同步：**根据解封装模块处理过程中获取到的参数信息，同步解码出来的视频和音频数据，并将视频音频数据送至系统的显卡和声卡播放出来。



# YUV格式分析

[YUV格式分析详解](https://blog.csdn.net/wudebao5220150/article/details/13295603)

[YUV介绍](https://www.cnblogs.com/sddai/p/10302979.html)

[详解YUV颜色体系](https://www.jianshu.com/p/e4944a2707bc)


# FFmpeg时间戳详解
[FFmpeg时间戳详解](https://www.cnblogs.com/leisure_chn/p/10584910.html)



# 博客学习

[音视频编解码](https://blog.csdn.net/wanggao_1990/category_10981719.html)

