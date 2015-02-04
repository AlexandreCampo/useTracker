
#include "Dilation.h"

// #include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

Dilation::Dilation() : PipelinePlugin()
{
}

void Dilation::Reset()
{
//    multithreaded = true;
    SetSize(size);
    result = Mat::zeros (pipeline->height, pipeline->width, CV_8U);
}

void Dilation::Apply()
{
    /// Apply the dilation operation
    dilate (pipeline->marked, result, structuringElement);
    result.copyTo(pipeline->marked);
}

void Dilation::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	size = (int)fn["Size"];
    }
}

void Dilation::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Size" << size;
}

void Dilation::SetSize (int s)
{
    size = s;

    structuringElement = getStructuringElement(
	MORPH_ELLIPSE,
	Size (2 * size + 1, 2 * size + 1),
	Point (size, size)
	);
}

