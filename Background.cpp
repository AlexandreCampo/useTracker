
#include "Background.h"
#include <opencv2/imgproc/imgproc.hpp>

// All the new API is put into "cv" namespace
using namespace cv;
using namespace std;


Mat CalculateBackgroundMedian (Capture* capture, float startTime, float endTime, unsigned int framesCount)
{
    int width = capture->width;
    int height = capture->height;
    Mat background (height, width, CV_8UC3);
    Mat frame;

    // do not calc bg for an image...
    if (capture->type == Capture::IMAGE) return background;

    // if endtime is very small, consider that we want to use the entire capture duration. In case 
    // this a webcam, give up calculation
    if (endTime < 0.01)
    {
	if (capture->type == Capture::VIDEO)
	{
	    endTime = ((double)capture->GetFrameCount()) / capture->fps;
	}
	else
	    return background;
    }

    cerr << "Calculating median background" << endl;

    unsigned char* samples = new unsigned char [framesCount * width * height * 3];

    unsigned int readCount = 0;
    unsigned int index = 0;
    float intervalTime = (endTime - startTime) / framesCount;
    while (readCount < framesCount)
    {
	capture->GetFrame (startTime + intervalTime * readCount);
//    	video.set(CV_CAP_PROP_POS_MSEC, (startTime + intervalTime * readCount) * 1000.0);

//    	float msgTime = float(video.get(CV_CAP_PROP_POS_MSEC) / 1000.0);
	double msgTime = capture->time;
    	cerr << "using frame at time " << msgTime << endl;

    	//video >> frame;

    	if (capture->frame.empty())
    	    break;

    	for (int y = 0; y < height; y++)
    	{
    	    unsigned char* frameRow = capture->frame.ptr<unsigned char>(y);

    	    for (int x = 0; x < width; x++)
    	    {
    		samples[index++] = frameRow[x * 3];
    		samples[index++] = frameRow[x * 3 + 1];
    		samples[index++] = frameRow[x * 3 + 2];
    	    }
    	}
    	readCount++;
    }

    // ok finished extracting frames from movie
    // automatically calculate background

    // for every pixel :
    int index2 = 0;
    for (int y = 0; y < height; y++)
    {
    	unsigned char* bgRow = background.ptr<unsigned char>(y);

    	for (int x = 0; x < width; x++)
    	{
    	    // gather some stats in a buffer
    	    unsigned char r[readCount];
    	    unsigned char g[readCount];
    	    unsigned char b[readCount];
    	    int idx = index2;
    	    for (unsigned int i = 0; i < readCount; i++)
    	    {
    		r[i] = samples[idx];
    		g[i] = samples[idx + 1];
    		b[i] = samples[idx + 2];

    		idx += width * height * 3;
    	    }
    	    index2 += 3;

    	    // now find out the most representative values
    	    unsigned char bgr = 0, bgg = 0, bgb = 0;
    	    unsigned char bgrCount = 0, bggCount = 0, bgbCount = 0;
    	    char hist[256];

    	    memset (hist, 0, 256);
    	    for (unsigned int i = 0; i < readCount; i++) hist[r[i]]++;
    	    for (unsigned int i = 0; i < 256; i++)
    	    {
    		if (hist[i] > bgrCount)
    		{
    		    bgrCount = hist[i]; bgr = i;
    		}
    	    }

    	    memset (hist, 0, 256);
    	    for (unsigned int i = 0; i < readCount; i++) hist[g[i]]++;
    	    for (unsigned int i = 0; i < 256; i++)
    	    {
    		if (hist[i] > bggCount)
    		{
    		    bggCount = hist[i]; bgg = i;
    		}
    	    }

    	    memset (hist, 0, 256);
    	    for (unsigned int i = 0; i < readCount; i++) hist[b[i]]++;
    	    for (unsigned int i = 0; i < 256; i++)
    	    {
    		if (hist[i] > bgbCount)
    		{
    		    bgbCount = hist[i]; bgb = i;
    		}
    	    }

    	    // now go through the triplets and select the one that is the closest from this evaluation
    	    unsigned int mindist = 1000000;
    	    unsigned char fr = 0, fg = 0, fb = 0;
    	    for (unsigned int i = 0; i < readCount; i++)
    	    {
    		unsigned int dist = abs(int(r[i]) - int(bgr)) + abs(int(g[i]) - int(bgg)) + abs(int(b[i]) - int(bgb));
    		if (dist < mindist)
    		{
    		    mindist = dist;
    		    fr = r[i];
    		    fg = g[i];
    		    fb = b[i];
    		}
    	    }

    	    // and at last record result in the bg buffer
    	    bgRow[x*3] = fr;
    	    bgRow[x*3+1] = fg;
    	    bgRow[x*3+2] = fb;
    	}
    }

    delete[] samples;
    cerr << "Background calculated" << endl;

    capture->Stop();
//    video.set(CV_CAP_PROP_POS_MSEC, 0.0);

    return background;
}



Mat CalculateBackgroundMean (Capture* capture, float startTime, float endTime, unsigned int framesCount)
{
    int width = capture->width;
    int height = capture->height;
    Mat background (height, width, CV_8UC3);
    Mat accu (height, width, CV_32FC3);
    Mat frame;

    // do not calc bg for an image...
    if (capture->type == Capture::IMAGE) return background;

    // if endtime is very small, consider that we want to use the entire capture duration. In case 
    // this a webcam, give up calculation
    if (endTime < 0.01)
    {
	if (capture->type == Capture::VIDEO)
	{
	    endTime = ((double)capture->GetFrameCount()) / capture->fps;
	}
	else
	    return background;
    }

    cerr << "Calculating mean background" << endl;

    unsigned int readCount = 0;
    float intervalTime = (endTime - startTime) / framesCount;
    while (readCount < framesCount)
    {
	capture->GetFrame(startTime + intervalTime * readCount);

//    	video.set(CV_CAP_PROP_POS_MSEC, (startTime + intervalTime * readCount) * 1000.0);

	//  	float msgTime = float(video.get(CV_CAP_PROP_POS_MSEC) / 1000.0);
	double msgTime = capture->time;
    	cerr << "using frame at time " << msgTime << endl;

    	//video >> frame;

    	if (capture->frame.empty())
    	    break;

    	cv::accumulate (capture->frame, accu);

    	readCount++;
    }

    accu /= readCount;

    accu.convertTo(background,CV_8UC3);

    cerr << "Background calculated" << endl;

//    video.set(CV_CAP_PROP_POS_MSEC, 0.0);
    capture->Stop();

    return background;
}

