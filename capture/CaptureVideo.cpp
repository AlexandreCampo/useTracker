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

#include "CaptureVideo.h"

#include <iostream>
#include <wx/time.h>
#include <cmath>

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

string CaptureVideo::GetName()
{
    return filename;
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
    if (fps <= 0.000001 || std::isnan(fps)) fps = 1;

    playSpeed = 0;
    playTimestep.Assign(1000000.0 / fps);
    isPaused = true;

    cout << "detected w/h/fps " << width << " " << height << " " << fps << std::endl;

    // read first frame to allow display
    Mat prevFrame = frame;
    source >> frame;
    calibration.Undistort(frame);

    GetFrameNumber();

    return (!frame.empty());
}

void CaptureVideo::Close ()
{

}

bool CaptureVideo::GetNextFrame ()
{
    Mat previousFrame = frame;
    source >> frame;
    calibration.Undistort(frame);

    GetFrameNumber();

    nextFrameTime += playTimestep;

    if (frame.empty())
    {
	frame = previousFrame;
	return false;
    }
    return true;
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

    Mat previousFrame = frame;
    source >> frame;
    calibration.Undistort(frame);

    GetFrameNumber();
    nextFrameTime += playTimestep;
    statusChanged = true;

    if (frame.empty())
    {
	frame = previousFrame;
	return false;
    }
    return true;
}

bool CaptureVideo::GetPreviousFrame()
{
    int f = GetFrameNumber();
    bool ok = SetFrameNumber(f-2);

    GetFrameNumber();
    nextFrameTime += playTimestep;

    return ok;
}

void CaptureVideo::Stop()
{
    source.set(CV_CAP_PROP_POS_FRAMES, 0);
    source >> frame;
    calibration.Undistort(frame);

    isStopped = true;
    statusChanged = true;
    GetFrameNumber();
}

bool CaptureVideo::SetFrameNumber(long frameNumber)
{
    source.set(CV_CAP_PROP_POS_FRAMES, frameNumber);

    Mat previousFrame = frame;    
    source >> frame;
    calibration.Undistort(frame);

    statusChanged = true;

    nextFrameTime += playTimestep;
    GetFrameNumber();

    if (frame.empty())
    {
	frame = previousFrame;
	return false;
    }
    return true;
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
    fs << "Type" << "video";
    fs << "Filename" << filename;

    if (calibration.calibrated)
    {
	fs << "Calibration" << "{";

	calibration.SaveXML(fs);

	fs << "}";
    }
}

void CaptureVideo::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
	filename = (string)fn["Filename"];

	FileNode calibNode = fn ["Calibration"];
	if (!calibNode.empty())
	{
	    calibration.LoadXML (calibNode);
	}
    }
}
