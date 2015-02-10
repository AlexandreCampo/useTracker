
#include "CaptureUSBCamera.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>

#include <wx/time.h>

using namespace std;
using namespace cv;

CaptureUSBCamera::CaptureUSBCamera(int device) : Capture()
{
    if (Open(device))
	type = USB_CAMERA;
}

CaptureUSBCamera::CaptureUSBCamera(FileNode& fn) : Capture()
{
    LoadXML(fn);
    if (Open(device))
	type = USB_CAMERA;
}

CaptureUSBCamera::~CaptureUSBCamera()
{

}

bool CaptureUSBCamera::Open (int device)
{
    this->device = device;
    source.open(device);

    // Check if the camera was opened
    if(!source.isOpened())
    {
	std::cerr << "Could not create capture" << std::endl;
	return false;
    }

    source.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
    source.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);

    fps = source.get(CV_CAP_PROP_FPS);

    // manually estimate fps (opencv bug workaround)
    if (fps <= 0.000001)
    {
	for (int i = 0; i < 10; i++) source >> frame;
	wxLongLong t1 = wxGetUTCTimeUSec();
	for (int i = 0; i < 10; i++) source >> frame;
	wxLongLong t2 = wxGetUTCTimeUSec();
	double delay = (t2 - t1).ToDouble() / 1000000.0;
	fps = 10.0 / delay;
    }

    width = source.get(CV_CAP_PROP_FRAME_WIDTH);
    height = source.get(CV_CAP_PROP_FRAME_HEIGHT);
    cout << "set to w/h/fps " << width << " " << height << " " << fps << std::endl;

    playTimestep.Assign(1000000.0 / fps);
    startTime = wxGetUTCTimeUSec();
    pauseTime = startTime;
    isPaused = true;

    source >> frame;

    return (!frame.empty());
}

void CaptureUSBCamera::Close ()
{

}

bool CaptureUSBCamera::GetNextFrame ()
{
    source >> frame;

    frameNumber++;
    nextFrameTime += playTimestep;

    return !frame.empty();
}

wxLongLong CaptureUSBCamera::GetNextFrameSystemTime()
{
    return nextFrameTime;
}

void CaptureUSBCamera::Stop()
{
    isPaused = false;
    isStopped = true;    
    statusChanged = true;
    frameNumber = 0;
}

void CaptureUSBCamera::Pause()
{
    isPaused = true;
    statusChanged = true;
    pauseTime = wxGetUTCTimeUSec();
}

void CaptureUSBCamera::Play()
{
    if (isPaused)
    {
	startTime += wxGetUTCTimeUSec() - pauseTime;
	nextFrameTime = wxGetUTCTimeUSec() + playTimestep;
	statusChanged = true;
	isPaused = false;
    }
    if (isStopped)
    {
	startTime = wxGetUTCTimeUSec();
	nextFrameTime = startTime + playTimestep;
	statusChanged = true;
	isStopped =false;	
    }
}

bool CaptureUSBCamera::GetFrame (double time)
{
    while (GetTime() < time) this_thread::sleep_for(chrono::milliseconds(10));
    source >> frame;

    frameNumber++;
    nextFrameTime = wxGetUTCTimeUSec() + playTimestep;

    return !frame.empty();
}

long CaptureUSBCamera::GetFrameNumber ()
{
    return frameNumber;
//    return source.get(CV_CAP_PROP_POS_FRAMES);
}

long CaptureUSBCamera::GetFrameCount ()
{
    return 1;
}

double CaptureUSBCamera::GetTime()
{
    if (isPaused) return (pauseTime - startTime).ToDouble() / 1000000.0;
    else if (isStopped) return 0;
    else return (wxGetUTCTimeUSec() - startTime).ToDouble() / 1000000.0;
}

void CaptureUSBCamera::SaveXML(FileStorage& fs)
{
    fs << "Source" << "{" << "Type";
    fs << "USBcamera" << "Device" << device;
    fs << "}";
}

void CaptureUSBCamera::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
	device = (int)fn["Device"];
    }
}
