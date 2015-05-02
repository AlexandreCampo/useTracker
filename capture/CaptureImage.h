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

#ifndef CAPTURE_IMAGE_H
#define CAPTURE_IMAGE_H

#include "Capture.h"

#include <string>

struct CaptureImage : public Capture
{
    std::string filename;

    CaptureImage(std::string filename);
    CaptureImage(cv::FileNode& fn);
    ~CaptureImage();

    bool Open(std::string filename);
    void Close();

    void Pause() {};
    void Play() {};
    void Stop() {};

    bool GetNextFrame ();
    wxLongLong GetNextFrameSystemTime();
    bool GetFrame (double time);

    long GetFrameNumber();
    long GetFrameCount();
    double GetTime();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    std::string GetName();
};




#endif //CAPTURE_IMAGE_H
