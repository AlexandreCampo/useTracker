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

#ifndef ADAPTIVE_THRESHOLD_H
#define ADAPTIVE_THRESHOLD_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>


class AdaptiveThreshold : public PipelinePlugin
{
public:

    int thresholdMethod = cv::ADAPTIVE_THRESH_MEAN_C;
    int blockSize = 1;
    int constant = 0;

    int zone = 0;
    bool restrictToZone = false;
    bool additive = false;

    cv::Mat diff;
    cv::Mat sum;
    cv::Mat marked2;
    cv::Mat marked3;

    AdaptiveThreshold();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void SetBlockSize(int bs);
    void SetConstant(int c);
    void SetThresholdMethod(int m);
	
};

#endif
