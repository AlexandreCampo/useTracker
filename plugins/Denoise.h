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

#ifndef DENOISE_PLUGIN_H
#define DENOISE_PLUGIN_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

// Spatial (single-frame) denoising. Enhancement plugin, applied in place.
// Bilateral keeps edges while smoothing noise (fast). NL-means gives cleaner
// results on textured noise but is slower.
class Denoise : public PipelinePlugin
{
public:

    enum Method {BILATERAL = 0, NLMEANS = 1};

    int method = BILATERAL;

    // bilateral
    int diameter = 5;          // neighbourhood diameter (0 = derived from sigma)
    float sigmaColor = 50.0f;  // colour similarity
    float sigmaSpace = 50.0f;  // spatial extent

    // NL-means
    float nlmStrength = 3.0f;      // luminance filter strength (h)
    float nlmColorStrength = 3.0f; // colour filter strength

    cv::Mat tmp;

    Denoise();
    ~Denoise();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif
