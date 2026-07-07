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

#ifndef DEHAZE_PLUGIN_H
#define DEHAZE_PLUGIN_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>

// Dark Channel Prior dehazing. Enhancement plugin, applied in place. Removes
// the veiling haze / backscatter that washes out contrast (turbid water,
// fog). Pairs well with the Underwater White Balance mode.
class Dehaze : public PipelinePlugin
{
public:

    float omega = 0.95f;   // how much haze to remove (0-1); higher = stronger
    int patchSize = 15;    // dark-channel patch (larger = smoother, less local)
    float t0 = 0.1f;       // minimum transmission (guards very hazy regions)

    Dehaze();
    ~Dehaze();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif
