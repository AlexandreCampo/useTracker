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

#include "FrameDifference.h"


#include "ImageProcessingEngine.h"

using namespace cv;

FrameDifference::FrameDifference () : PipelinePlugin()
{
    multithreaded = true;
}

void FrameDifference::Reset()
{
    previousMarked = Mat::zeros (pipeline->height, pipeline->width, CV_8UC1);
    diffMarked = Mat::zeros (pipeline->height, pipeline->width, CV_8UC1);
}

void FrameDifference::Apply()
{
    if (!pipeline->parent->staticFrame)
    {
	previousMarked = diffMarked;
	Mat tmp = pipeline->marked.clone();
	for (int i = 0; i < pipeline->width*pipeline->height; i++)
	{
	    pipeline->marked.data[i] = (~diffMarked.data[i]) & pipeline->marked.data[i];
	}
	diffMarked = tmp;
    }
    else
    {
	for (int i = 0; i < pipeline->width*pipeline->height; i++)
	{
	    pipeline->marked.data[i] = (~previousMarked.data[i]) & pipeline->marked.data[i];
	}
    }
}


void FrameDifference::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
   }
}

void FrameDifference::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
}
