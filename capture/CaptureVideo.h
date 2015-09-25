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

#ifndef CAPTURE_VIDEO_H
#define CAPTURE_VIDEO_H

#include "Capture.h"

struct CaptureVideo : Capture
{
    std::string filename;
    cv::VideoCapture source;

    unsigned int frameNumber = 0;
    wxLongLong startTime = 0;
    wxLongLong nextFrameTime = 0;
    wxLongLong playTimestep = 0;
    wxLongLong time = 0;
    int playSpeed = 0;


    CaptureVideo(std::string filename);
    CaptureVideo(cv::FileNode& fn);
    ~CaptureVideo();

    bool Open(std::string filename);
    void Close();

    bool GetNextFrame ();
    bool GetPreviousFrame();
    wxLongLong GetNextFrameSystemTime();

    bool GetFrame (double time);

//    void Start();
    void Stop();

    void Play();
    void Pause();

    void SetSpeedFaster(int speed);
    void SetSpeedSlower(int speed);

    long GetFrameNumber();
    long GetFrameCount();
    bool SetFrameNumber(long f);

    void SetTime(double time);
    double GetTime();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    std::string GetName();
};




#endif //CAPTURE_VIDEO_H
