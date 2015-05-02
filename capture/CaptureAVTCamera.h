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

#ifdef VIMBA

#ifndef CAPTURE_AVT_CAMERA_H
#define CAPTURE_AVT_CAMERA_H

#include "Capture.h"

#include "vimba/ApiController.h"

class AVT::VmbAPI::ApiController;

struct CaptureAVTCamera : Capture
{
    int device;
    AVT::VmbAPI::ApiController vimbaApiController;

    unsigned int frameNumber = 0;
    wxLongLong startTime = 0;
    wxLongLong pauseTime = 0;
    wxLongLong lastFrameTime = 0;
    wxLongLong nextFrameTime = 0;
    wxLongLong playTimestep = 0;

    CaptureAVTCamera(int device);
    CaptureAVTCamera(cv::FileNode& fn);
    ~CaptureAVTCamera();

    bool Open(int device);
    void Close();

    void Pause();
    void Play();
    void Stop();

    bool GetNextFrame ();
    bool GetFrame (double time);
    wxLongLong GetNextFrameSystemTime();
    wxLongLong InternalGetTime();
    double GetTime();

    long GetFrameNumber();
    long GetFrameCount();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    std::string GetName();
};




#endif //CAPTURE_AVT_CAMERA_H

#endif // VIMBA
