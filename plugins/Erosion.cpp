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

#include "Erosion.h"

using namespace cv;
using namespace std;

Erosion::Erosion () : PipelinePlugin()
{
}

void Erosion::Reset()
{
//    multithreaded = true;
    SetSize(size);
    result = Mat::zeros (pipeline->height, pipeline->width, CV_8U);
}

void Erosion::Apply()
{
    /// Apply the dilation operation
    erode (pipeline->marked, result, structuringElement);
    result.copyTo(pipeline->marked);
}

void Erosion::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	size = (int)fn["Size"];
    }
}

void Erosion::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Size" << size;
}

void Erosion::SetSize (int s)
{
    size = s;

    structuringElement = getStructuringElement(
	MORPH_ELLIPSE,
	Size (2 * size + 1, 2 * size + 1),
	Point (size, size)
	);
}
