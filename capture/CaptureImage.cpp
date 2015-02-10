
#include "CaptureImage.h"

using namespace std;
using namespace cv;

CaptureImage::CaptureImage(string filename) : Capture()
{
    if (Open(filename))
	type = IMAGE;
}

CaptureImage::CaptureImage(FileNode& fn) : Capture()
{
    LoadXML(fn);
    if (Open(filename))
	type = IMAGE;
}

CaptureImage::~CaptureImage()
{

}

bool CaptureImage::Open (string filename)
{
    this->filename = filename;
    frame = imread (filename.c_str());

    width = frame.cols;
    height = frame.rows;
    fps = 1;

    return (!frame.empty());
}

void CaptureImage::Close ()
{

}

bool CaptureImage::GetNextFrame ()
{
    return false; // no other frame available
}

wxLongLong CaptureImage::GetNextFrameSystemTime()
{
    return 0;
}

bool CaptureImage::GetFrame (double time)
{
    return false;
}

long CaptureImage::GetFrameNumber ()
{
    return 0;
}

long CaptureImage::GetFrameCount ()
{
    return 1;
}

double CaptureImage::GetTime()
{
    return 0;
}

void CaptureImage::SaveXML(FileStorage& fs)
{
    fs << "Source" << "{" << "Type";
    fs << "image" << "Filename" << filename;
    fs << "}";
}

void CaptureImage::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
	filename = (string)fn["Filename"];
    }
}
