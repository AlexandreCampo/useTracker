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

#ifndef CURVES_PLUGIN_H
#define CURVES_PLUGIN_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#include <vector>

// Tone / colour curves. A general image-enhancement plugin: each of the
// Master, Red, Green and Blue channels has an editable transfer curve
// (control points interpolated with a monotone cubic spline into a lookup
// table). The Master curve is applied to all three channels, then the
// per-channel curve. Curves subsume levels, gamma, contrast and colour
// balance in one tool. Like the other enhancement plugins it modifies the
// capture frame in place, so the effect is visible live and feeds downstream.
class Curves : public PipelinePlugin
{
public:

    enum Channel { MASTER = 0, RED = 1, GREEN = 2, BLUE = 3, NUM_CHANNELS = 4 };

    // control points per channel, in [0,255] x [0,255], sorted by x, with the
    // first point at x=0 and the last at x=255
    std::vector<cv::Point2f> points[NUM_CHANNELS];

    int editChannel = MASTER;   // channel currently shown in the editor (UI)
    bool lutDirty = true;

    cv::Mat lutMat;             // 1x256 CV_8UC3 combined lookup table

    Curves();
    ~Curves();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void SetIdentity(int channel);
    void MarkDirty() { lutDirty = true; }
    void BuildLUT();

    // sample a curve's control points into a 256-entry lookup table using a
    // monotone cubic spline (no overshoot). Exposed for the GUI editor.
    static void BuildCurveLUT (const std::vector<cv::Point2f>& pts, unsigned char out[256]);
};

#endif
