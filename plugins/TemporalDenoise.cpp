/*----------------------------------------------------------------------------*/
/*    Copyright (C) 2015 Alexandre Campo                                      */
/*                                                                            */
/*    This file is part of USE Tracker.                                       */
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

#include "TemporalDenoise.h"

#include "ImageProcessingEngine.h"

#include <vector>

using namespace cv;
using namespace std;

TemporalDenoise::TemporalDenoise() : PipelinePlugin()
{
    // needs the whole frame and keeps global temporal state
    multithreaded = false;
}

TemporalDenoise::~TemporalDenoise()
{
}

void TemporalDenoise::Reset()
{
    lastResult.release();
}

int TemporalDenoise::PrefetchAhead()
{
    int n = windowSize;
    if (n < 3) n = 3;
    return n / 2;   // radius of future frames needed
}

void TemporalDenoise::Apply()
{
    if (pipeline->frame.channels() != 3) return;

    // odd window >= 3
    int n = windowSize;
    if (n < 3) n = 3;
    if ((n % 2) == 0) n += 1;
    int r = n / 2;

    bool staticFrame = pipeline->parent->staticFrame;

    // gather a window centred on the current frame from the engine's shared
    // prefetch buffer (past on one side, future on the other)
    std::vector<Mat> imgs;
    imgs.reserve(n);
    for (int k = -r; k <= r; k++)
    {
	Mat m = pipeline->parent->GetBufferedImage(k);
	if (m.empty()) return; // buffer not primed yet: passthrough
	imgs.push_back(m);
    }

    // denoise the centre frame (index r); recompute only on a real new frame,
    // reuse the last result while paused
    if (!staticFrame || lastResult.empty())
	fastNlMeansDenoisingColoredMulti(imgs, lastResult, r, n,
					 strength, colorStrength, 7, 21);

    if (!lastResult.empty())
	lastResult.copyTo(pipeline->frame);
}

void TemporalDenoise::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	if (!fn["WindowSize"].empty()) windowSize = (int)fn["WindowSize"];
	if (!fn["Strength"].empty()) strength = (float)fn["Strength"];
	if (!fn["ColorStrength"].empty()) colorStrength = (float)fn["ColorStrength"];

	if (windowSize < 3) windowSize = 3;
    }
}

void TemporalDenoise::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "WindowSize" << windowSize;
    fs << "Strength" << strength;
    fs << "ColorStrength" << colorStrength;
}
