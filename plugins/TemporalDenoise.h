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

#ifndef TEMPORAL_DENOISE_H
#define TEMPORAL_DENOISE_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

// Temporal (multi-frame) denoising with OpenCV's fastNlMeansDenoisingColored-
// Multi. Keeps a rolling buffer of recent frames and denoises the CENTRE frame
// of that window, using the frames on both sides (past and, relative to that
// centre frame, future). Because it outputs the centre of the window, its
// result is delayed by (window-1)/2 frames but has no positional lag on moving
// objects. Reduces noise using temporal redundancy without motion blur.
// Applied in place. NOTE: NL-means is CPU-heavy - expect slow playback; best
// for offline analysis.
class TemporalDenoise : public PipelinePlugin
{
public:

    int windowSize = 5;        // odd; number of frames used (radius = (n-1)/2)
    float strength = 3.0f;     // luminance filter strength (h)
    float colorStrength = 3.0f;

    cv::Mat lastResult;

    TemporalDenoise();
    ~TemporalDenoise();
    void Apply();
    void Reset();
    int PrefetchAhead();       // ask the engine to prefetch (window-1)/2 frames
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif
