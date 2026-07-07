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

#ifndef CLAHE_PLUGIN_H
#define CLAHE_PLUGIN_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

// Contrast Limited Adaptive Histogram Equalization. Image enhancement
// plugin: modifies the capture frame in place so that the effect is
// visible live and benefits all downstream plugins. The engine restores
// the pristine source frame on static re-steps (and RecordVideo records
// it), so the enhancement is applied exactly once per frame.
class Clahe : public PipelinePlugin
{
public:

    float clipLimit = 3.0f;
    int tileSize = 8;

    cv::Ptr<cv::CLAHE> clahe;
    cv::Mat lab;
    std::vector<cv::Mat> channels;

    Clahe();
    ~Clahe();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void SetClipLimit(float c);
    void SetTileSize(int s);
};

#endif
