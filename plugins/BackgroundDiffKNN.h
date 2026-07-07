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

#ifndef BACKGROUND_DIFF_KNN_H
#define BACKGROUND_DIFF_KNN_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video/background_segm.hpp>


class BackgroundDiffKNN : public PipelinePlugin
{
public:

    int history = 500;
    double threshold = 400.0;  // squared distance threshold
    bool shadowDetection = false;
    double learningRate = -1.0; // -1 = automatic

    int zone = 0;
    bool restrictToZone = false;
    bool additive = false;

    cv::Mat marked2;
    cv::Mat marked3;

    cv::Ptr<cv::BackgroundSubtractorKNN> KNN;

    BackgroundDiffKNN();
    ~BackgroundDiffKNN();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void SetHistory(int h);
    void SetThreshold(double t);
    void SetShadowDetection(bool s);
};

#endif
