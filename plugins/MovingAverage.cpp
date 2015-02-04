
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
    length = 1;
    threshold = 1;
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
