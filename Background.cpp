/*----------------------------------------------------------------------------*/
/*    Copyright (C) 2015 Alexandre Campo                                      */
/*                                                                            */
/*    This file is part of USE Tracker.                                       */
/*                                                                            */
/*    USE Tracker is free software: you can redistribute it and/or modify     */
/*    it under the terms of the GNU General Public License as published by    */
/*    the Free Software Foundation, either version 3 of the License, or       */
/*    (at your option) any later version.                                     */
/*                                                                            */
/*    USE Tracker is distributed in the hope that it will be useful,          */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/*    GNU General Public License for more details.                            */
/*                                                                            */
/*    You should have received a copy of the GNU General Public License       */
/*    along with USE Tracker.  If not, see <http://www.gnu.org/licenses/>.    */
/*----------------------------------------------------------------------------*/

#include "Background.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;


Mat CalculateBackgroundMedian (Capture* capture, float startTime, float endTime, unsigned int framesCount)
{
    int width = capture->width;
    int height = capture->height;
    Mat background = Mat::zeros(height, width, CV_8UC3);

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

    capture->Stop();
    capture->Play();

    unsigned char* samples = new unsigned char [framesCount * width * height * 3];

    unsigned int readCount = 0;
    unsigned int index = 0;
    float intervalTime = (endTime - startTime) / framesCount;
    while (readCount < framesCount)
    {
	capture->GetFrame (startTime + intervalTime * readCount);

	double msgTime = capture->GetTime();
    	cerr << "using frame at time " << msgTime << endl;

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

    capture->Stop();

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

    return background;
}



Mat CalculateBackgroundMean (Capture* capture, float startTime, float endTime, unsigned int framesCount)
{
    int width = capture->width;
    int height = capture->height;
    Mat background = Mat::zeros(height, width, CV_8UC3);
    Mat accu = Mat::zeros(height, width, CV_32FC3);

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

    capture->Stop();
    capture->Play();

    unsigned int readCount = 0;
    float intervalTime = (endTime - startTime) / framesCount;
    while (readCount < framesCount)
    {
	capture->GetFrame(startTime + intervalTime * readCount);

	double msgTime = capture->GetTime();
    	cerr << "using frame at time " << msgTime << endl;

    	if (capture->frame.empty())
    	    break;

    	cv::accumulate (capture->frame, accu);

    	readCount++;
    }

    accu /= readCount;

    accu.convertTo(background,CV_8UC3);

    cerr << "Background calculated" << endl;

    capture->Stop();

    return background;
}

