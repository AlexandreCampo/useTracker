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

#ifndef CAPTURE_DEFAULT_H
#define CAPTURE_DEFAULT_H

#include "Capture.h"

#include <string>

struct CaptureDefault : public Capture
{
    CaptureDefault();

    bool GetNextFrame ();
    wxLongLong GetNextFrameSystemTime ();
    bool GetFrame (double time);

    void Pause() {};
    void Play() {};
    void Stop() {};

    long GetFrameNumber();
    long GetFrameCount();
    double GetTime();

    void Close() {};
    void LoadXML (cv::FileNode& fn) {};
    void SaveXML (cv::FileStorage& fs) {};

    std::string GetName() {return std::string();};
};




#endif //CAPTURE_DEFAULT_H
