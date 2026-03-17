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

#include "MovingAverage.h"

#include "ImageProcessingEngine.h"

using namespace cv;

MovingAverage::MovingAverage () : PipelinePlugin()
{
    multithreaded = true;
}

void MovingAverage::Reset()
{
    movingAverage = Mat::zeros (pipeline->height, pipeline->width, CV_8UC1);
    framesSum = Mat::zeros (pipeline->height, pipeline->width, CV_32F);
    oldFrame = Mat::zeros (pipeline->height, pipeline->width, CV_32F);
}

MovingAverage::~MovingAverage ()
{
}

void MovingAverage::Apply()
{
    if (!pipeline->parent->staticFrame)
    {
	// record marked in the queue
	frames.push(pipeline->marked.clone());

	// add current frame to the sum
	cv::accumulate (pipeline->marked, framesSum);//, pipeline->zoneMap);

	// if buffer full, calculate moving average
	if (frames.size() > length)
	{
	    Mat& old = frames.front();
	    old.convertTo (oldFrame, CV_32F);
	    framesSum -= oldFrame;

	    inRange(framesSum, threshold * 255, length * 255, movingAverage);

	    frames.pop();
	}
	// not enough frames accumulated, return blank 
	else
	{
	    movingAverage.setTo(0);
	}
    }

    movingAverage.copyTo(pipeline->marked);
}

void MovingAverage::SetLength(unsigned int l)
{
    while (frames.size() > l)
    {
	Mat& old = frames.front();
	old.convertTo (oldFrame, CV_32F);
	framesSum -= oldFrame;
	frames.pop();
    }
    length = l;
}

void MovingAverage::ClearHistory()
{
    while (!frames.empty()) 
	frames.pop();

    framesSum.setTo(0);
}

void MovingAverage::SetThreshold(unsigned int t)
{
    threshold = t;
}

void MovingAverage::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	length = (int)fn["Length"];
	threshold = (int)fn["Threshold"];
    }
}

void MovingAverage::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Length" << (int)length;
    fs << "Threshold" << (int)threshold;
}
