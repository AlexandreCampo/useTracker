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

#ifndef COLOR_SEGMENTATION_H
#define COLOR_SEGMENTATION_H

#include "PipelinePlugin.h"

#include <opencv2/opencv.hpp>

class ColorSegmentation : public PipelinePlugin
{
public:
    enum Type {BGR,HSV};

    Type type = HSV;

    cv::Mat frameHSV;
    cv::Mat marked2;
    cv::Vec3b minBGR = {0,0,0};
    cv::Vec3b maxBGR = {255,255,255};
    cv::Vec3b minHSV = {0,0,0};
    cv::Vec3b maxHSV = {180,255,255};

    cv::Mat pixelSrc;
    cv::Mat pixelDst;
    cv::Mat pixelRes;
    cv::Mat pixelRes2;

    ColorSegmentation();
    void Reset();
    void Apply();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif
