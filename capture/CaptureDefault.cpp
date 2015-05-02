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

#include "CaptureDefault.h"

using namespace std;
using namespace cv;

CaptureDefault::CaptureDefault() : Capture()
{
    frame = Mat::zeros(600, 800, CV_8UC3);

    int baseline = 0;
    char str[] = "USE Tracker";
    Size textSize = getTextSize(str, FONT_HERSHEY_TRIPLEX, 2.0, 3, &baseline);
    Point pos (frame.cols / 2 - textSize.width / 2, frame.rows / 2 + textSize.height / 2);
    putText(frame, str, pos, FONT_HERSHEY_TRIPLEX, 2.0, cvScalar(64,64,64), 3, CV_AA);

    width = frame.cols;
    height = frame.rows;
    fps = 1;

    type = IMAGE;
}

bool CaptureDefault::GetNextFrame ()
{
    return false;
}

wxLongLong CaptureDefault::GetNextFrameSystemTime()
{
    return 0;
}

bool CaptureDefault::GetFrame (double time)
{
    return false;
}

long CaptureDefault::GetFrameNumber ()
{
    return 0;
}

long CaptureDefault::GetFrameCount ()
{
    return 1;
}

double CaptureDefault::GetTime()
{
    return 0;
}
