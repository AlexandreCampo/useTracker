
#include "CaptureVideo.h"

#include <iostream>
#include <wx/time.h>

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

    playSpeed = 0;
    playTimestep = (wxLongLong)(1000000.0 / fps);
    isPaused = true;

    cout << "detected w/h/fps " << width << " " << height << " " << fps << std::endl;

    // read first frame to allow display
    source >> frame;

    GetFrameNumber();

    return (!frame.empty());
}

void CaptureVideo::Close ()
{

}

bool CaptureVideo::GetNextFrame ()
{
    source >> frame;

    GetFrameNumber();

    nextFrameTime += playTimestep;

    return !frame.empty();
}

wxLongLong CaptureVideo::GetNextFrameSystemTime()
{
    return nextFrameTime;
}

void CaptureVideo::Pause()
{
    isPaused = true;
    statusChanged = true;
}

void CaptureVideo::Play()
{
    // restart timing
    if (isPaused || isStopped)
    {
	nextFrameTime = wxGetUTCTimeUSec() + playTimestep;
	isPaused = false;
	isStopped = false;
	statusChanged = true;
    }    
}



bool CaptureVideo::GetFrame (double time)
{
    source.set(CV_CAP_PROP_POS_MSEC, time * 1000.0);
    source >> frame;

    GetFrameNumber();
    nextFrameTime += playTimestep;
    statusChanged = true;

    return !frame.empty();
}

bool CaptureVideo::GetPreviousFrame()
{
    int f = GetFrameNumber();
    SetFrameNumber(f-2);

    GetFrameNumber();
    nextFrameTime += playTimestep;

    return (!frame.empty());
}

void CaptureVideo::Stop()
{
    source.set(CV_CAP_PROP_POS_FRAMES, 0);
    source >> frame;

    isStopped = true;
    statusChanged = true;
    GetFrameNumber();
}

void CaptureVideo::SetFrameNumber(long frameNumber)
{
    source.set(CV_CAP_PROP_POS_FRAMES, frameNumber);

    source >> frame;
    statusChanged = true;

    nextFrameTime += playTimestep;
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

// related to movie time
// void CaptureVideo::SetTime(double time)
// {
//     source.set(CV_CAP_PROP_POS_MSEC, time * 1000.0);
//     source >> frame;

//     nextFrameTime += playTimestep;
//     GetFrameNumber();
// }

void CaptureVideo::SetSpeedFaster(int speed)
{
    if (speed > 1) playTimestep.Assign(1000000.0 / (fps * speed));
    else playTimestep.Assign(1000000.0 / fps);
    nextFrameTime = wxGetUTCTimeUSec() + playTimestep;
}

void CaptureVideo::SetSpeedSlower(int speed)
{
    if (speed > 1) playTimestep.Assign(1000000.0 * speed / fps);
    else playTimestep.Assign(1000000.0 / fps);
    nextFrameTime = wxGetUTCTimeUSec() + playTimestep;
}

// related to movie time
double CaptureVideo::GetTime()
{
    return (double)source.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
    //return time;
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
