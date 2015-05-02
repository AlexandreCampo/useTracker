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

#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include "PipelinePlugin.h"

#include <queue>
#include <opencv2/imgproc/imgproc.hpp>

class MovingAverage : public PipelinePlugin
{
public:
    std::queue<cv::Mat> frames;
    cv::Mat movingAverage;
    cv::Mat framesSum;
    cv::Mat oldFrame;
    unsigned int length = 10;
    unsigned int threshold = 5;

    MovingAverage();
    ~MovingAverage();

    void Reset();
    void Apply();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void SetLength(unsigned int l);
    void SetThreshold(unsigned int t);

};

#endif

