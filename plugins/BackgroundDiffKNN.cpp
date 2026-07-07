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

#include "BackgroundDiffKNN.h"

#include "ImageProcessingEngine.h"

using namespace cv;

BackgroundDiffKNN::BackgroundDiffKNN() : PipelinePlugin()
{
    multithreaded = true;
}

BackgroundDiffKNN::~BackgroundDiffKNN()
{
}

void BackgroundDiffKNN::Reset()
{
    marked2 = Mat::zeros(pipeline->height, pipeline->width, CV_8U);
    marked3 = Mat(pipeline->height, pipeline->width, CV_8U);

    KNN = createBackgroundSubtractorKNN(history, threshold, shadowDetection);

    // prime the model only if a real background image is available: seeding
    // a sample-based subtractor with a blank image poisons the model (the
    // whole frame is marked as foreground for hundreds of frames)
    if (cv::sum(pipeline->background) != cv::Scalar::all(0))
	KNN->apply(pipeline->background, marked2);
}

// KNN parameters can be changed on the fly, no model reset needed

void BackgroundDiffKNN::SetHistory(int h)
{
    if (h < 1) h = 1;
    history = h;
    if (KNN) KNN->setHistory(h);
}

void BackgroundDiffKNN::SetThreshold(double t)
{
    if (t < 1.0) t = 1.0;
    threshold = t;
    if (KNN) KNN->setDist2Threshold(t);
}

void BackgroundDiffKNN::SetShadowDetection(bool s)
{
    shadowDetection = s;
    if (KNN) KNN->setDetectShadows(s);
}

void BackgroundDiffKNN::Apply()
{
    // do not update the background model on a static frame (paused or
    // replayed), the model history must only come from actual movie frames
    double lr = pipeline->parent->staticFrame ? 0.0 : learningRate;
    KNN->apply(pipeline->frame, marked2, lr);

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

void BackgroundDiffKNN::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	history = (int)fn["History"];
	threshold = (float)fn["Threshold"];
	shadowDetection = (int)fn["ShadowDetection"];
	if (!fn["LearningRate"].empty())
	    learningRate = (float)fn["LearningRate"];

	if (history <= 0) history = 500;
	if (threshold <= 0.0) threshold = 400.0;

	restrictToZone = (int)fn["RestrictToZone"];
	if (restrictToZone)
	    zone = (int)fn["Zone"];
	additive = (int)fn["Additive"];
    }
}

void BackgroundDiffKNN::SaveXML (FileStorage& fs)
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
