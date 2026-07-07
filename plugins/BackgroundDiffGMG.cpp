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

#include "BackgroundDiffGMG.h"

#include "ImageProcessingEngine.h"

using namespace cv;
using namespace cv::bgsegm;

BackgroundDiffGMG::BackgroundDiffGMG() : PipelinePlugin()
{
    multithreaded = true;
}

BackgroundDiffGMG::~BackgroundDiffGMG()
{
}

void BackgroundDiffGMG::Reset()
{
    diff = Mat(pipeline->height, pipeline->width, CV_8UC3);
    sum = Mat(pipeline->height, pipeline->width, CV_8U);
    marked2 = Mat(pipeline->height, pipeline->width, CV_8U);
    marked3 = Mat(pipeline->height, pipeline->width, CV_8U);

    GMG = createBackgroundSubtractorGMG();
    GMG->apply(pipeline->background, marked2);
}


void BackgroundDiffGMG::Apply()
{
    // do not update the background model on a static frame (paused or replayed),
    // the model history must only come from actual movie frames
    double lr = pipeline->parent->staticFrame ? 0.0 : learningRate;
    GMG->apply(pipeline->frame, marked2, lr);

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

void BackgroundDiffGMG::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	learningRate = (float)fn["LearningRate"];

	restrictToZone = (int)fn["RestrictToZone"];
	if (restrictToZone)
	    zone = (int)fn["Zone"];
	additive = (int)fn["Additive"];
    }
}

void BackgroundDiffGMG::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "LearningRate" << learningRate;

    fs << "RestrictToZone" << restrictToZone;
    if (restrictToZone)
	fs << "Zone" << zone;
    fs << "Additive" << additive;
}
