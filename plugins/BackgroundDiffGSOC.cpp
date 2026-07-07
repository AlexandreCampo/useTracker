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

#include "BackgroundDiffGSOC.h"

#include "ImageProcessingEngine.h"

using namespace cv;
using namespace cv::bgsegm;

BackgroundDiffGSOC::BackgroundDiffGSOC() : PipelinePlugin()
{
    multithreaded = true;
}

BackgroundDiffGSOC::~BackgroundDiffGSOC()
{
}

void BackgroundDiffGSOC::Reset()
{
    marked2 = Mat::zeros(pipeline->height, pipeline->width, CV_8U);
    marked3 = Mat(pipeline->height, pipeline->width, CV_8U);

    // enforce the constraints of the GSOC implementation, invalid values
    // trigger a fatal assertion in OpenCV
    if (nSamples < 2) nSamples = 2;
    if (nSamples > 1023) nSamples = 1023;
    if (replaceRate < 0.0f) replaceRate = 0.0f;
    if (replaceRate > 1.0f) replaceRate = 1.0f;
    if (propagationRate < 0.0f) propagationRate = 0.0f;
    if (propagationRate > 1.0f) propagationRate = 1.0f;
    if (hitsThreshold < 0) hitsThreshold = 0;

    try
    {
	GSOC = createBackgroundSubtractorGSOC(LSBP_CAMERA_MOTION_COMPENSATION_NONE,
					      nSamples, replaceRate, propagationRate, hitsThreshold);
    }
    catch (const cv::Exception& e)
    {
	std::cerr << "BackgroundDiffGSOC: could not create model: " << e.what() << std::endl;
	GSOC = createBackgroundSubtractorGSOC();
    }

    // prime the model only if a real background image is available: seeding
    // a sample-based subtractor with a blank image poisons the model (the
    // whole frame is marked as foreground for hundreds of frames)
    if (cv::sum(pipeline->background) != cv::Scalar::all(0))
	GSOC->apply(pipeline->background, marked2);
}

void BackgroundDiffGSOC::SetNSamples(int n)
{
    if (n < 2) n = 2;
    if (n > 1023) n = 1023;
    if (n == nSamples) return; // avoid needless model reset

    nSamples = n;
    Reset();
}

void BackgroundDiffGSOC::SetReplaceRate(float r)
{
    if (r < 0.0f) r = 0.0f;
    if (r > 1.0f) r = 1.0f;
    if (r == replaceRate) return; // avoid needless model reset

    replaceRate = r;
    Reset();
}

void BackgroundDiffGSOC::SetPropagationRate(float r)
{
    if (r < 0.0f) r = 0.0f;
    if (r > 1.0f) r = 1.0f;
    if (r == propagationRate) return; // avoid needless model reset

    propagationRate = r;
    Reset();
}

void BackgroundDiffGSOC::SetHitsThreshold(int t)
{
    if (t < 0) t = 0;
    if (t == hitsThreshold) return; // avoid needless model reset

    hitsThreshold = t;
    Reset();
}

void BackgroundDiffGSOC::Apply()
{
    // GSOC has no learning rate control: skip the model update entirely on
    // static frames (paused or replayed) and reuse the last computed mask,
    // the model history must only come from actual movie frames
    if (!pipeline->parent->staticFrame)
	GSOC->apply(pipeline->frame, marked2);

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

void BackgroundDiffGSOC::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	nSamples = (int)fn["NSamples"];
	replaceRate = (float)fn["ReplaceRate"];
	propagationRate = (float)fn["PropagationRate"];
	hitsThreshold = (int)fn["HitsThreshold"];

	if (nSamples < 2) nSamples = 20;
	if (replaceRate <= 0.0f) replaceRate = 0.003f;
	if (propagationRate <= 0.0f) propagationRate = 0.01f;
	if (hitsThreshold <= 0) hitsThreshold = 32;

	restrictToZone = (int)fn["RestrictToZone"];
	if (restrictToZone)
	    zone = (int)fn["Zone"];
	additive = (int)fn["Additive"];
    }
}

void BackgroundDiffGSOC::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "NSamples" << nSamples;
    fs << "ReplaceRate" << replaceRate;
    fs << "PropagationRate" << propagationRate;
    fs << "HitsThreshold" << hitsThreshold;

    fs << "RestrictToZone" << restrictToZone;
    if (restrictToZone)
	fs << "Zone" << zone;
    fs << "Additive" << additive;
}
