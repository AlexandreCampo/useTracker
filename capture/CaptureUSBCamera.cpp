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

    if(!source.isOpened())
    {
	std::cerr << "Could not create capture" << std::endl;
	return false;
    }

    source.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    source.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    fps = source.get(cv::CAP_PROP_FPS);

    // manually estimate fps (opencv bug workaround)
    if (fps <= 0.000001 || std::isnan(fps)) fps = 1;
    {
	for (int i = 0; i < 10; i++) source >> frame;
	int64_t t1 = GetUTCTimeUSec();
	for (int i = 0; i < 10; i++) source >> frame;
	int64_t t2 = GetUTCTimeUSec();
	double delay = (double)(t2 - t1) / 1000000.0;
	fps = 10.0 / delay;
    }

    width = (int)source.get(cv::CAP_PROP_FRAME_WIDTH);
    height = (int)source.get(cv::CAP_PROP_FRAME_HEIGHT);
    cout << "set to w/h/fps " << width << " " << height << " " << fps << std::endl;

    playTimestep = (int64_t)(1000000.0 / fps);
    startTime = GetUTCTimeUSec();
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

int64_t CaptureUSBCamera::GetNextFrameSystemTime()
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
    pauseTime = GetUTCTimeUSec();
}

void CaptureUSBCamera::Play()
{
    if (isPaused)
    {
	startTime += GetUTCTimeUSec() - pauseTime;
	nextFrameTime = GetUTCTimeUSec() + playTimestep;
	statusChanged = true;
	isPaused = false;
    }
    if (isStopped)
    {
	startTime = GetUTCTimeUSec();
	nextFrameTime = startTime + playTimestep;
	statusChanged = true;
	isStopped = false;
    }
}

bool CaptureUSBCamera::GetFrame (double time)
{
    time *= 1000000.0;
    while (InternalGetTime() < (int64_t)time) this_thread::sleep_for(chrono::milliseconds(10));

    Mat previousFrame = frame;

    // take several frames otherwise we get an old buffered frame
    for (int i = 0; i < 4; i++)
	source >> frame;

    calibration.Undistort(frame);

    frameNumber++;
    lastFrameTime = InternalGetTime();
    nextFrameTime = GetUTCTimeUSec() + playTimestep;

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
}

long CaptureUSBCamera::GetFrameCount ()
{
    return 1;
}

double CaptureUSBCamera::GetTime()
{
    if (isStopped) return 0;
    else return (double)lastFrameTime / 1000000.0;
}

int64_t CaptureUSBCamera::InternalGetTime()
{
    if (isPaused) return (pauseTime - startTime);
    else if (isStopped) return 0;
    else return (GetUTCTimeUSec() - startTime);
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
