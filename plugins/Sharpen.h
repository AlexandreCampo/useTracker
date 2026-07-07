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

#ifndef SHARPEN_PLUGIN_H
#define SHARPEN_PLUGIN_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>

// Unsharp-mask sharpening. Enhancement plugin, applied in place. Boosts edges
// and fine detail by adding back the difference between the image and a
// blurred copy of it. Best used after a denoise stage.
class Sharpen : public PipelinePlugin
{
public:

    float amount = 1.0f;   // sharpening strength
    float radius = 2.0f;   // blur sigma (larger = coarser detail enhanced)

    cv::Mat blurred;

    Sharpen();
    ~Sharpen();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif
