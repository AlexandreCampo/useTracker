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

#include "CaptureImage.h"

using namespace std;
using namespace cv;

CaptureImage::CaptureImage(string filename) : Capture()
{
    if (Open(filename))
	type = IMAGE;
}

CaptureImage::CaptureImage(FileNode& fn) : Capture()
{
    LoadXML(fn);
    if (Open(filename))
	type = IMAGE;
}

CaptureImage::~CaptureImage()
{

}

string CaptureImage::GetName()
{
    return filename;
}

bool CaptureImage::Open (string filename)
{
    this->filename = filename;
    frame = imread (filename.c_str());

    width = frame.cols;
    height = frame.rows;
    fps = 1;

    return (!frame.empty());
}

void CaptureImage::Close ()
{

}

bool CaptureImage::GetNextFrame ()
{
    return false; // no other frame available
}

wxLongLong CaptureImage::GetNextFrameSystemTime()
{
    return 0;
}

bool CaptureImage::GetFrame (double time)
{
    return false;
}

long CaptureImage::GetFrameNumber ()
{
    return 0;
}

long CaptureImage::GetFrameCount ()
{
    return 1;
}

double CaptureImage::GetTime()
{
    return 0;
}

void CaptureImage::SaveXML(FileStorage& fs)
{
    fs << "Source" << "{" << "Type";
    fs << "image" << "Filename" << filename;
    fs << "}";
}

void CaptureImage::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
	filename = (string)fn["Filename"];
    }
}
