#include <iostream>
#include "XVideoWriter.h"
using namespace std;
int main()
{
	char outfile[] = "rgbpcm.mp4";
	char rgbfile[] = "test.rgb";
	char pcmfile[] = "test.pcm";
	XVideoWriter* xw = XVideoWriter::Get(0);
	cout << xw->Init(outfile) << endl;
	cout << xw->ADDVideoStream() << endl;
	xw->ADDAudioStream();
	FILE* fp = fopen(rgbfile, "rb");
	if (!fp)
	{
		cout << "open file " << rgbfile << " fail!\n";
		return -1;
	}
	cout << "open file " << rgbfile << " success!\n";

	FILE* fa = fopen(pcmfile, "rb");
	if (!fp)
	{
		cout << "open file " << pcmfile << " fail!\n";
		return -1;
	}
	cout << "open file " << pcmfile << " success!\n";

	int size = xw->inWidth * xw->inHeight * 4;
	unsigned char* rgb = new unsigned char[size];
	int asize = xw->nb_sample * xw->inChannels * 2;
	unsigned char* pcm = new unsigned char[size];
	
	xw->WriteHead();
	for(;;)
	{
		AVPacket* pkt = NULL;
		int len = NULL;
		if (xw->isVideoBefore())
		{
			len = fread(rgb, 1, size, fp);
			if (len <= 0)break;
			pkt = xw->EncodeVideo(rgb);
			if (pkt)
			{
				cout << ".";
			}
			else
			{
				cout << "-";
				continue;
			}
			if (xw->WriteFrame(pkt))
			{
				cout << "+";
			}
		}
		else
		{
			len = fread(pcm, 1, asize, fa);
			if (len <= 0)break;
			pkt = xw->EncodeAudio(pcm);
			xw->WriteFrame(pkt);
		}
		

		
		
	}
	xw->WriteEnd();
	delete rgb;
	rgb = NULL;
	return 0;
}
