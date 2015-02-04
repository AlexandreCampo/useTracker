
#include "Erosion.h"

using namespace cv;
using namespace std;

Erosion::Erosion () : PipelinePlugin()
{
}

void Erosion::Reset()
{
//    multithreaded = true;
    SetSize(size);
    result = Mat::zeros (pipeline->height, pipeline->width, CV_8U);
}

void Erosion::Apply()
{
    /// Apply the dilation operation
    erode (pipeline->marked, result, structuringElement);
    result.copyTo(pipeline->marked);
}

void Erosion::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	size = (int)fn["Size"];
    }
}

void Erosion::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Size" << size;
}

void Erosion::SetSize (int s)
{
    size = s;

    structuringElement = getStructuringElement(
	MORPH_ELLIPSE,
	Size (2 * size + 1, 2 * size + 1),
	Point (size, size)
	);
}
