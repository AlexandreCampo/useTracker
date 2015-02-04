
#include "ExtractMotion.h"


using namespace cv;

ExtractMotion::ExtractMotion() : PipelinePlugin()
{
    multithreaded = true;
}

void ExtractMotion::Reset()
{
    diff = Mat(pipeline->height, pipeline->width, CV_8UC3);
    sum = Mat(pipeline->height, pipeline->width, CV_8U);
    marked2 = Mat(pipeline->height, pipeline->width, CV_8U);
}


void ExtractMotion::Apply()
{
    absdiff (pipeline->frame, pipeline->background, diff);
    cvtColor(diff, sum, CV_BGR2GRAY);
    cv::threshold(sum, marked2, threshold, 255, THRESH_BINARY);

    pipeline->marked &= marked2;
}

void ExtractMotion::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	threshold = (int)fn["Threshold"];
    }
}

void ExtractMotion::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Threshold" << threshold;
}

