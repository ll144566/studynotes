#include <iostream>
extern "C"
{
#include <libavformat/avformat.h>
}
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
using namespace std;
void Xexit(const char * info)
{
	cout << info << endl;
	exit(1);
}
int main()
{
	char infile[] = "test.mp4";
	char outfile[] = "test.mov";
	av_register_all();
	AVFormatContext* ic = NULL;
	avformat_open_input(&ic, infile, 0, 0);
	if (!ic)
		Xexit("avformat_open_input failed");
	cout << "open " << infile << endl;
	
	AVFormatContext* oc;
	avformat_alloc_output_context2(&oc, NULL, NULL, outfile);
	if (!oc)
		Xexit("avformat_alloc_output_context2 failed");

	AVStream *videoStream = avformat_new_stream(oc, NULL);
	AVStream *audioStream = avformat_new_stream(oc, NULL);
	avcodec_parameters_copy(videoStream->codecpar, ic->streams[0]->codecpar);
	avcodec_parameters_copy(audioStream->codecpar, ic->streams[1]->codecpar);
	videoStream->codecpar->codec_tag = 0;
	audioStream->codecpar->codec_tag = 0;


	cout << "============================================\n";
	av_dump_format(ic, 0, infile, 0);
	cout << "============================================\n";
	av_dump_format(oc, 0, outfile, 1);

	int ret = avio_open(&oc->pb, outfile, AVIO_FLAG_WRITE);
	if (ret < 0)
		Xexit("avio_open failed");
	ret = avformat_write_header(oc, NULL);
	if(ret < 0)
		Xexit("avformat_write_header failed");

	AVPacket pkt;
	for (;;)
	{
		ret = av_read_frame(ic, &pkt);
		if (ret < 0)
			break;
		pkt.pts = av_rescale_q_rnd(
			pkt.pts,
			ic->streams[pkt.stream_index]->time_base,
			oc->streams[pkt.stream_index]->time_base,
			(AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
		);
		pkt.dts = av_rescale_q_rnd(
			pkt.dts,
			ic->streams[pkt.stream_index]->time_base,
			oc->streams[pkt.stream_index]->time_base,
			(AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
		);
		pkt.pos = -1;
		pkt.duration = av_rescale_q_rnd(
			pkt.duration,
			ic->streams[pkt.stream_index]->time_base,
			oc->streams[pkt.stream_index]->time_base,
			(AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
		);
		av_write_frame(oc, &pkt);

		av_packet_unref(&pkt);
		cout << ".";
	}

	av_write_trailer(oc);
	avio_close(oc->pb);
	return 0;
}
