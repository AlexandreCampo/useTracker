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

#ifndef ARUCO_COLOR_H
#define ARUCO_COLOR_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <arucoColor/arucoColor.h>


class ArucoColor : public PipelinePlugin
{
public:


    
    // instantiate aruco color lib/main object
    ac::ArucoColor libac;
    
    int size = 1;
    cv::Mat structuringElement;
    cv::Mat result;
    
    ArucoColor();

    void Reset();
    void Apply();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void OutputHud (cv::Mat& hud);

    void SetSize(int s);
};

#endif
