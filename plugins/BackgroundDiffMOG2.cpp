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

#include "BackgroundDiffMOG2.h"


using namespace cv;

BackgroundDiffMOG2::BackgroundDiffMOG2() : PipelinePlugin()
{
    multithreaded = true;
}

BackgroundDiffMOG2::~BackgroundDiffMOG2()
{
#if CV_MAJOR_VERSION == 2
    delete MOG2;
#endif
}

void BackgroundDiffMOG2::Reset()
{
    diff = Mat(pipeline->height, pipeline->width, CV_8UC3);
    sum = Mat(pipeline->height, pipeline->width, CV_8U);
    marked2 = Mat(pipeline->height, pipeline->width, CV_8U);
    marked3 = Mat(pipeline->height, pipeline->width, CV_8U);

#if CV_MAJOR_VERSION == 2
    delete MOG2;
    MOG2 = new BackgroundSubtractorMOG2(history, threshold, shadowDetection);    
    MOG2->operator()(pipeline->background, marked2);
#else
    MOG2 = createBackgroundSubtractorMOG2(history, threshold, shadowDetection);
    MOG2->apply(pipeline->background, marked2);
#endif
}


void BackgroundDiffMOG2::SetHistory(int h)
{
    history = h;
    
#if CV_MAJOR_VERSION == 2
    delete MOG2;
    MOG2 = new BackgroundSubtractorMOG2(history, threshold, shadowDetection);    
    MOG2->operator()(pipeline->background, marked2);
#else
    MOG2 = createBackgroundSubtractorMOG2(history, threshold, shadowDetection);
    MOG2->apply(pipeline->background, marked2);
#endif
}

void BackgroundDiffMOG2::SetThreshold(double t)
{
    threshold = t;
    
#if CV_MAJOR_VERSION == 2
    delete MOG2;
    MOG2 = new BackgroundSubtractorMOG2(history, threshold, shadowDetection);    
    MOG2->operator()(pipeline->background, marked2);
#else
    MOG2 = createBackgroundSubtractorMOG2(history, threshold, shadowDetection);
    MOG2->apply(pipeline->background, marked2);
#endif
}

void BackgroundDiffMOG2::SetShadowDetection(bool s)
{
    shadowDetection = s;
    
#if CV_MAJOR_VERSION == 2
    delete MOG2;
    MOG2 = new BackgroundSubtractorMOG2(history, threshold, shadowDetection);    
    MOG2->operator()(pipeline->background, marked2);
#else
    MOG2 = createBackgroundSubtractorMOG2(history, threshold, shadowDetection);
    MOG2->apply(pipeline->background, marked2);
#endif
}


void BackgroundDiffMOG2::Apply()
{    
#if CV_MAJOR_VERSION == 2
    MOG2->operator()(pipeline->frame, marked2, learningRate);
#else
    MOG2->apply(pipeline->frame, marked2, learningRate);    
#endif
    
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

void BackgroundDiffMOG2::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	history = (int)fn["History"];
	threshold = (float)fn["Threshold"];
	shadowDetection = (int)fn["ShadowDetection"];
	learningRate = (float)fn["LearningRate"];

	restrictToZone = (int)fn["RestrictToZone"];
	if (restrictToZone)
	    zone = (int)fn["Zone"];
	additive = (int)fn["Additive"];
    }
}

void BackgroundDiffMOG2::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "History" << history;
    fs << "Threshold" << threshold;
    fs << "ShadowDetection" << shadowDetection;
    fs << "LearningRate" << learningRate;

    fs << "RestrictToZone" << restrictToZone;
    if (restrictToZone)
	fs << "Zone" << zone;
    fs << "Additive" << additive;
}

