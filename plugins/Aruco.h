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

#ifndef ARUCO_H
#define ARUCO_H

#include "PipelinePlugin.h"

#include <iostream>
#include <fstream>
#include <memory>

#include <opencv2/imgproc.hpp>
#include <opencv2/core/version.hpp>

// OpenCV 4.7+ moved aruco into objdetect with a new class-based API
#if CV_VERSION_MAJOR > 4 || (CV_VERSION_MAJOR == 4 && CV_VERSION_MINOR >= 7)
#define ARUCO_NEW_API 1
#include <opencv2/objdetect/aruco_detector.hpp>
#else
#define ARUCO_NEW_API 0
#include <opencv2/aruco.hpp>
#endif

#define ARUCO_MASK_SHAPE_NONE 0
#define ARUCO_MASK_SHAPE_SQUARE 1
#define ARUCO_MASK_SHAPE_DISC 2

class Aruco : public PipelinePlugin
{
public:

#if ARUCO_NEW_API
    cv::aruco::Dictionary dictionary;
    cv::aruco::DetectorParameters detectorParams;
    std::unique_ptr<cv::aruco::ArucoDetector> detector;
#else
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
#endif

    std::vector<std::vector<cv::Point2f>> markerCorners;
    std::vector<int> markerIds;

    double minSize = 0.001;
    double maxSize = 0.1;

    int thresh1 = 19;
    int thresh2 = 7;

    int maskShape = ARUCO_MASK_SHAPE_NONE;
    int maskRadius = 1;
    int maskPerspectiveShift = 0;
    int maskValue = 0;

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

    void SetMaskShape(int v);
    void SetMaskRadius(int v);
    void SetMaskPerspectiveShift(int v);
    void SetMaskValue(int v);

private:
    void RebuildDetector();
};

#endif
