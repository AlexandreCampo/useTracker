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

#include "BackgroundDiffMOG.h"

#include "ImageProcessingEngine.h"

using namespace cv;
using namespace cv::bgsegm;

BackgroundDiffMOG::BackgroundDiffMOG() : PipelinePlugin()
{
    multithreaded = true;
}

BackgroundDiffMOG::~BackgroundDiffMOG()
{
}

void BackgroundDiffMOG::Reset()
{
    diff = Mat(pipeline->height, pipeline->width, CV_8UC3);
    sum = Mat(pipeline->height, pipeline->width, CV_8U);
    marked2 = Mat(pipeline->height, pipeline->width, CV_8U);
    marked3 = Mat(pipeline->height, pipeline->width, CV_8U);

    MOG = createBackgroundSubtractorMOG(history, nMixtures, backgroundRatio, noiseSigma);
    MOG->apply(pipeline->background, marked2);
}


void BackgroundDiffMOG::SetHistory(int h)
{
    if (h < 1) h = 1;
    if (h == history) return; // avoid needless model reset

    history = h;

    MOG = createBackgroundSubtractorMOG(history, nMixtures, backgroundRatio, noiseSigma);
    MOG->apply(pipeline->background, marked2);
}

void BackgroundDiffMOG::SetNMixtures(int m)
{
    if (m < 1) m = 1;
    if (m > 8) m = 8;
    if (m == nMixtures) return; // avoid needless model reset

    nMixtures = m;

    MOG = createBackgroundSubtractorMOG(history, nMixtures, backgroundRatio, noiseSigma);
    MOG->apply(pipeline->background, marked2);
}

void BackgroundDiffMOG::SetBackgroundRatio(double r)
{
    if (r == backgroundRatio) return; // avoid needless model reset

    backgroundRatio = r;

    MOG = createBackgroundSubtractorMOG(history, nMixtures, backgroundRatio, noiseSigma);
    MOG->apply(pipeline->background, marked2);
}

void BackgroundDiffMOG::SetNoiseSigma(double s)
{
    if (s == noiseSigma) return; // avoid needless model reset

    noiseSigma = s;

    MOG = createBackgroundSubtractorMOG(history, nMixtures, backgroundRatio, noiseSigma);
    MOG->apply(pipeline->background, marked2);
}

void BackgroundDiffMOG::Apply()
{
    // do not update the background model on a static frame (paused or replayed),
    // the model history must only come from actual movie frames
    double lr = pipeline->parent->staticFrame ? 0.0 : learningRate;
    MOG->apply(pipeline->frame, marked2, lr);

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

void BackgroundDiffMOG::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	history = (int)fn["History"];
	nMixtures = (int)fn["nMixtures"];
	backgroundRatio = (float)fn["BackgroundRatio"];
	noiseSigma = (float)fn["NoiseSigma"];
	learningRate = (float)fn["LearningRate"];

	restrictToZone = (int)fn["RestrictToZone"];
	if (restrictToZone)
	    zone = (int)fn["Zone"];
	additive = (int)fn["Additive"];
    }
}

void BackgroundDiffMOG::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "History" << history;
    fs << "nMixtures" << nMixtures;
    fs << "BackgroundRatio" << backgroundRatio;
    fs << "NoiseSigma" << noiseSigma;
    fs << "LearningRate" << learningRate;

    fs << "RestrictToZone" << restrictToZone;
    if (restrictToZone)
	fs << "Zone" << zone;
    fs << "Additive" << additive;
}
