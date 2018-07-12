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

#include "AdaptiveThreshold.h"


using namespace cv;

AdaptiveThreshold::AdaptiveThreshold() : PipelinePlugin()
{
    multithreaded = true;
}

void AdaptiveThreshold::Reset()
{
    diff = Mat(pipeline->height, pipeline->width, CV_8UC3);
    sum = Mat(pipeline->height, pipeline->width, CV_8U);
    marked2 = Mat(pipeline->height, pipeline->width, CV_8U);
    marked3 = Mat(pipeline->height, pipeline->width, CV_8U);
}


void AdaptiveThreshold::Apply()
{
    absdiff (pipeline->frame, pipeline->background, diff);
    cvtColor(diff, sum, CV_BGR2GRAY);
    cv::adaptiveThreshold(sum, marked2, 255, thresholdMethod, THRESH_BINARY, blockSize*2+1, constant);

    if (invert)
        cv::bitwise_not(marked2, marked2);
    
    if (restrictToZone)
    {
	cv::inRange(pipeline->zoneMap, zone, zone, marked3);
	marked2 &= marked3;	    
    }
    
    if (additive)	
	pipeline->marked |= (marked2 & pipeline->zoneMap);
    else
	pipeline->marked &= marked2;
}

void AdaptiveThreshold::SetBlockSize(int bs)
{
    this->blockSize = bs;
}

void AdaptiveThreshold::SetConstant(int c)
{
    this->constant = c;
}

void AdaptiveThreshold::SetThresholdMethod(int m)
{
    if (m == 0) this->thresholdMethod = ADAPTIVE_THRESH_MEAN_C;
    else if (m == 1) this->thresholdMethod = ADAPTIVE_THRESH_GAUSSIAN_C;
}

void AdaptiveThreshold::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];

	string tm = (string)fn["ThresholdMethod"];	
	if (tm == "Mean")
	    thresholdMethod = ADAPTIVE_THRESH_MEAN_C;
	else if (tm == "Gaussian")
	    thresholdMethod = ADAPTIVE_THRESH_GAUSSIAN_C;

	blockSize = (float)fn["BlockSize"];
	constant = (float)fn["Constant"];
	invert = (int)fn["Invert"];

	restrictToZone = (int)fn["RestrictToZone"];
	if (restrictToZone)
	    zone = (int)fn["Zone"];
	additive = (int)fn["Additive"];
    }
}

void AdaptiveThreshold::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;

    if (thresholdMethod == ADAPTIVE_THRESH_MEAN_C)
	fs << "ThresholdMethod" << "Mean";
    else if (thresholdMethod == ADAPTIVE_THRESH_GAUSSIAN_C)
	fs << "ThresholdMethod" << "Gaussian";
	
    fs << "BlockSize" << blockSize;
    fs << "Constant" << constant;
    fs << "Invert" << invert;
    
    fs << "RestrictToZone" << restrictToZone;
    if (restrictToZone)
	fs << "Zone" << zone;
    fs << "Additive" << additive;
}

