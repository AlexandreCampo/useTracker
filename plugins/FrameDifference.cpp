
#include "FrameDifference.h"


#include "ImageProcessingEngine.h"

using namespace cv;

FrameDifference::FrameDifference () : PipelinePlugin()
{
    multithreaded = true;
}

void FrameDifference::Reset()
{
    previousMarked = Mat::zeros (pipeline->height, pipeline->width, CV_8UC1);
    diffMarked = Mat::zeros (pipeline->height, pipeline->width, CV_8UC1);
}

void FrameDifference::Apply()
{
    if (!pipeline->parent->staticFrame)
    {
	previousMarked = diffMarked;
	Mat tmp = pipeline->marked.clone();
	for (int i = 0; i < pipeline->width*pipeline->height; i++)
	{
	    pipeline->marked.data[i] = (~diffMarked.data[i]) & pipeline->marked.data[i];
	}
	diffMarked = tmp;
    }
    else
    {
	for (int i = 0; i < pipeline->width*pipeline->height; i++)
	{
	    pipeline->marked.data[i] = (~previousMarked.data[i]) & pipeline->marked.data[i];
	}
    }
}


void FrameDifference::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
   }
}

void FrameDifference::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
}
