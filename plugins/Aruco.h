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

#ifdef ARUCO

#ifndef ARUCO_H
#define ARUCO_H

#include "PipelinePlugin.h"

#include <iostream>
#include <fstream>

#include <aruco/aruco.h>
#include <opencv2/imgproc/imgproc.hpp>

#define ARUCO_MASK_SHAPE_NONE 0
#define ARUCO_MASK_SHAPE_SQUARE 1
#define ARUCO_MASK_SHAPE_DISC 2

class Aruco : public PipelinePlugin
{
public:

    aruco::MarkerDetector detector;
    std::vector<aruco::Marker> markers;
    aruco::CameraParameters cameraParameters;

    double minSize = 0.001;
    double maxSize = 0.1;

    int thresh1 = 19;
    int thresh2 = 7;

    int maskShape = ARUCO_MASK_SHAPE_NONE;
    int maskRadius = 1;
    int maskPerspectiveShift = 0;
    int maskValue = 0;
    
    aruco::MarkerDetector::ThresholdMethods thresholdMethod = aruco::MarkerDetector::ADPT_THRES;

    std::string outputFilename;
    std::fstream outputStream;

    Aruco();
    ~Aruco();

    void Reset();
    void Apply();
    void OutputHud (cv::Mat& hud);
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void OutputStep();
    void OpenOutput();
    void CloseOutput();

    void SetMinSize(double minSize);
    void SetMaxSize(double maxSize);
    void SetThreshold1(int t1);
    void SetThreshold2(int t2);
    void SetThresholdMethod (int m);
    
    void SetMaskShape(int v);
    void SetMaskRadius(int v);
    void SetMaskPerspectiveShift(int v);
    void SetMaskValue(int v);
};



#endif

#endif
