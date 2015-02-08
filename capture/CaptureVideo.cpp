
#include "CaptureVideo.h"

#include <iostream>

using namespace std;
using namespace cv;


CaptureVideo::CaptureVideo(string filename) : Capture()
{
    if (Open (filename))
	type = VIDEO;
}

CaptureVideo::CaptureVideo(FileNode& fn) : Capture()
{
    LoadXML (fn);
    if (Open (filename))
	type = VIDEO;
}

CaptureVideo::~CaptureVideo()
{

}

bool CaptureVideo::Open (string filename)
{
    // open the video file
    this->filename = filename;
    source.open (filename.c_str());
    if (!source.isOpened())
    {
	std::cerr << "Could not open video source" << std::endl;
	return false;
    }

    width = source.get(CV_CAP_PROP_FRAME_WIDTH);
    height = source.get(CV_CAP_PROP_FRAME_HEIGHT);
    fps = source.get(CV_CAP_PROP_FPS);

    // fps incorrectly detected
    if (fps <= 0) fps = 1;

    cout << "detected w/h/fps " << width << " " << height << " " << fps << std::endl;

    // read first frame to allow display
    source >> frame;

    GetTime();
    GetFrameNumber();

    return (!frame.empty());
}

void CaptureVideo::Close ()
{

}

bool CaptureVideo::GetNextFrame (bool needed)
{
    source >> frame;

    GetFrameNumber();
    GetTime();

    return !frame.empty();
}

bool CaptureVideo::GetFrame (double time)
{
    source.set(CV_CAP_PROP_POS_MSEC, time * 1000.0);

    source >> frame;

    GetTime();
    GetFrameNumber();

    return !frame.empty();
}

bool CaptureVideo::GetPreviousFrame(bool needed)
{
    int f = GetFrameNumber();
    SetFrameNumber(f-2);

    GetTime();
    GetFrameNumber();

    return (!frame.empty());
}

void CaptureVideo::Stop()
{
    source.set(CV_CAP_PROP_POS_FRAMES, 0);
    source >> frame;

    GetTime();
    GetFrameNumber();
}

void CaptureVideo::SetFrameNumber(long frameNumber)
{
    source.set(CV_CAP_PROP_POS_FRAMES, frameNumber);

    source >> frame;

    GetTime();
    GetFrameNumber();
}

long CaptureVideo::GetFrameNumber ()
{
    frameNumber = source.get(CV_CAP_PROP_POS_FRAMES);
    return frameNumber;
}

long CaptureVideo::GetFrameCount ()
{
    return source.get(CV_CAP_PROP_FRAME_COUNT);
}

void CaptureVideo::SetTime(double time)
{
    source.set(CV_CAP_PROP_POS_MSEC, time * 1000.0);
    source >> frame;

    GetTime();
    GetFrameNumber();
}

double CaptureVideo::GetTime()
{
    time = (double)source.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
    return time;
}

void CaptureVideo::SaveXML(FileStorage& fs)
{
    fs << "Source" << "{" << "Type";
    fs << "video" << "Filename" << filename;
    fs << "}";
}

void CaptureVideo::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
	filename = (string)fn["Filename"];
    }
}
