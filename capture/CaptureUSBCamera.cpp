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

string CaptureUSBCamera::GetName()
{
    string str = "USB Camera device ";
    return str + std::to_string(device);
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
    if (fps <= 0.000001 || std::isnan(fps)) fps = 1;
//    if (fps <= 0.000001)
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
    calibration.Undistort(frame);
    lastFrameTime = InternalGetTime();

    return (!frame.empty());
}

void CaptureUSBCamera::Close ()
{

}

bool CaptureUSBCamera::GetNextFrame ()
{
    Mat previousFrame = frame;
    source >> frame;
    calibration.Undistort(frame);

    frameNumber++;
    lastFrameTime = InternalGetTime();
    nextFrameTime += playTimestep;

    if (frame.empty())
    {
	frame = previousFrame;
	return false;
    }
    return true;
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
    time *= 1000000.0;
    while (InternalGetTime() < time) this_thread::sleep_for(chrono::milliseconds(10));

    Mat previousFrame = frame;

    // take several frames otherwise we get an old buffered frame
    for (int i = 0; i < 4; i++)
	source >> frame;

    calibration.Undistort(frame);

    frameNumber++;
    lastFrameTime = InternalGetTime();
    nextFrameTime = wxGetUTCTimeUSec() + playTimestep;

    if (frame.empty())
    {
	frame = previousFrame;
	return false;
    }
    return true;
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
    if (isStopped) return 0;
    else return lastFrameTime.ToDouble() / 1000000.0;
}

wxLongLong CaptureUSBCamera::InternalGetTime()
{
    if (isPaused) return (pauseTime - startTime);
    else if (isStopped) return 0;
    else return (wxGetUTCTimeUSec() - startTime);
}

void CaptureUSBCamera::SaveXML(FileStorage& fs)
{
    fs << "Type" << "USBcamera";
    fs << "Device" << device;

    if (calibration.calibrated)
    {
	fs << "Calibration" << "{";

	calibration.SaveXML(fs);

	fs << "}";
    }
}

void CaptureUSBCamera::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
	device = (int)fn["Device"];

	FileNode calibNode = fn ["Calibration"];
	if (!calibNode.empty())
	{
	    calibration.LoadXML (calibNode);
	}
    }
}

