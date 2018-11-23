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

#ifndef BACKGROUND_DIFF_MOG_H
#define BACKGROUND_DIFF_MOG_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>

#if CV_MAJOR_VERSION == 3
#include <opencv2/bgsegm.hpp>
#endif


class BackgroundDiffMOG : public PipelinePlugin
{
public:

    int history = 200;
    int nMixtures = 5;
    double backgroundRatio = 0.7;
    double noiseSigma = 15.0;
    double learningRate = 0.05;
    
    int zone = 0;
    bool restrictToZone = false;
    bool additive = false;

    cv::Mat diff;
    cv::Mat sum;
    cv::Mat marked2;
    cv::Mat marked3;

#if CV_MAJOR_VERSION == 2
    cv::BackgroundSubtractor* MOG = NULL;
#else
    Ptr<BackgroundSubtractor> MOG;
#endif   
    
    BackgroundDiffMOG();
    ~BackgroundDiffMOG();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void SetHistory(int h);
    void SetNMixtures(int m);
    void SetBackgroundRatio(double r);
    void SetNoiseSigma(double s);
};

#endif
