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

#ifndef FRAME_DIFFERENCE_H
#define FRAME_DIFFERENCE_H

#include "PipelinePlugin.h"

#include <iostream>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>

class FrameDifference : public PipelinePlugin
{
public:

    bool usePipeline = true;
    int threshold = 1;

    int zone = 0;
    bool restrictToZone = false;
    bool additive = false;
    bool invert = false;

    cv::Mat previousFrame;
    cv::Mat diffFrame;
    cv::Mat sumFrame;
    cv::Mat previousMarked;
    cv::Mat currentMarked;
    cv::Mat marked2;
    cv::Mat marked3;

    FrameDifference();

    void SetUsePipeline(bool up);
    void SetThreshold(int t);

    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif

