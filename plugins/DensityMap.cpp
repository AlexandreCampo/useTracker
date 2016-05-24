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

#include "DensityMap.h"


using namespace cv;

DensityMap::DensityMap() : PipelinePlugin()
{
    multithreaded = true;
}

void DensityMap::Reset()
{
    sum = Mat(resolutionY, resolutionX, CV_32U);
}


void DensityMap::Apply()
{
    // add selected rect to accumulator
    

    absdiff (pipeline->frame, pipeline->background, diff);
    cvtColor(diff, sum, CV_BGR2GRAY);
    cv::threshold(sum, marked2, threshold, 255, THRESH_BINARY);

    pipeline->marked &= marked2;
}

void DensityMap::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	threshold = (int)fn["Threshold"];
    }
}

void DensityMap::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Threshold" << threshold;
}

