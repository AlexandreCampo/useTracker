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

#include "Aruco.h"

#include "Blob.h"
#include "ImageProcessingEngine.h"

using namespace cv;
using namespace std;

Aruco::Aruco() : PipelinePlugin()
{
#if ARUCO_NEW_API
    dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    RebuildDetector();
#else
    detectorParams = cv::aruco::DetectorParameters::create();
    dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
#endif
}

Aruco::~Aruco()
{
    CloseOutput();
}

void Aruco::RebuildDetector()
{
#if ARUCO_NEW_API
    detector = std::make_unique<cv::aruco::ArucoDetector>(dictionary, detectorParams);
#endif
}

void Aruco::Reset()
{
#if ARUCO_NEW_API
    detectorParams.adaptiveThreshWinSizeMin = thresh1;
    detectorParams.adaptiveThreshWinSizeMax = thresh1;
    detectorParams.adaptiveThreshConstant = thresh2;
    detectorParams.minMarkerPerimeterRate = minSize;
    detectorParams.maxMarkerPerimeterRate = maxSize;
    RebuildDetector();
#else
    detectorParams->adaptiveThreshWinSizeMin = thresh1;
    detectorParams->adaptiveThreshWinSizeMax = thresh1;
    detectorParams->adaptiveThreshConstant = thresh2;
    detectorParams->minMarkerPerimeterRate = minSize;
    detectorParams->maxMarkerPerimeterRate = maxSize;
#endif
}

void Aruco::Apply()
{
    std::vector<std::vector<cv::Point2f>> rejected;
#if ARUCO_NEW_API
    detector->detectMarkers(pipeline->frame, markerCorners, markerIds, rejected);
#else
    cv::aruco::detectMarkers(pipeline->frame, dictionary, markerCorners, markerIds, detectorParams, rejected);
#endif

    // when no mask shape, threshold the frame for the marked buffer
    if (maskShape == ARUCO_MASK_SHAPE_NONE)
    {
        cv::Mat grey;
        cv::cvtColor(pipeline->frame, grey, cv::COLOR_BGR2GRAY);
        cv::adaptiveThreshold(grey, pipeline->marked, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, thresh1 * 2 + 1, thresh2);
    }

    // clean blobs list
    vector<Blob>& blobs = pipeline->parent->blobs;
    blobs.clear();

    // fill the list with detected markers
    for (size_t i = 0; i < markerIds.size(); i++)
    {
        auto& corners = markerCorners[i];

        // compute center as average of 4 corners
        Point2f pos(0, 0);
        for (auto& c : corners) pos += c;
        pos *= 0.25f;

        Blob b(0, 0, 0);
        b.tagId = markerIds[i];
        b.x = pos.x;
        b.y = pos.y;
        b.size = (int)cv::contourArea(corners);
        Point2f dir = corners[1] - corners[0];
        b.angle = atan2(dir.y, dir.x);

        unsigned int idx = (int)b.x + (int)b.y * pipeline->width;
        if (idx < (unsigned int)(pipeline->width * pipeline->height))
            b.zone = pipeline->zoneMap.data[idx];

        blobs.push_back(b);
    }

    // if needed, draw markers in marked buffer
    if (maskShape != ARUCO_MASK_SHAPE_NONE)
    {
        for (size_t i = 0; i < markerIds.size(); i++)
        {
            auto& corners = markerCorners[i];
            if (corners.size() != 4) continue;

            Point2f pos(0, 0);
            for (auto& c : corners) pos += c;
            pos *= 0.25f;

            // calculate perspective shift
            Point2f center(pipeline->width / 2, pipeline->height / 2);
            Point2f vect = pos - center;
            float dist = norm(vect);
            float maxDist = norm(center);
            float shift = (float)maskPerspectiveShift * dist / maxDist;
            Point2f drawPos = vect * ((dist - shift) / dist) + center;

            if (maskShape == ARUCO_MASK_SHAPE_SQUARE)
            {
                Point2f points[4];
                for (int j = 0; j < 4; j++)
                    points[j] = corners[j] - pos;

                float d0 = norm(points[0]);
                float d1 = norm(points[1]);
                float d2 = norm(points[2]);
                float d3 = norm(points[3]);

                points[0] = points[0] * ((float)maskRadius / d0) + drawPos;
                points[1] = points[1] * ((float)maskRadius / d1) + drawPos;
                points[2] = points[2] * ((float)maskRadius / d2) + drawPos;
                points[3] = points[3] * ((float)maskRadius / d3) + drawPos;

                Point pts[4];
                for (int j = 0; j < 4; j++)
                    pts[j] = points[j];

                fillConvexPoly(pipeline->marked, pts, 4, maskValue);
            }
            else if (maskShape == ARUCO_MASK_SHAPE_DISC)
            {
                circle(pipeline->marked, drawPos, maskRadius, maskValue, cv::FILLED);
            }
        }
    }
}

void Aruco::OutputHud(Mat& hud)
{
    cv::aruco::drawDetectedMarkers(hud, markerCorners, markerIds);
}

void Aruco::SetMinSize(double minSize)
{
    this->minSize = minSize;
#if ARUCO_NEW_API
    detectorParams.minMarkerPerimeterRate = minSize;
    RebuildDetector();
#else
    detectorParams->minMarkerPerimeterRate = minSize;
#endif
}

void Aruco::SetMaxSize(double maxSize)
{
    this->maxSize = maxSize;
#if ARUCO_NEW_API
    detectorParams.maxMarkerPerimeterRate = maxSize;
    RebuildDetector();
#else
    detectorParams->maxMarkerPerimeterRate = maxSize;
#endif
}

void Aruco::SetThreshold1(int t1)
{
    this->thresh1 = t1;
#if ARUCO_NEW_API
    detectorParams.adaptiveThreshWinSizeMin = t1;
    detectorParams.adaptiveThreshWinSizeMax = t1;
    RebuildDetector();
#else
    detectorParams->adaptiveThreshWinSizeMin = t1;
    detectorParams->adaptiveThreshWinSizeMax = t1;
#endif
}

void Aruco::SetThreshold2(int t2)
{
    this->thresh2 = t2;
#if ARUCO_NEW_API
    detectorParams.adaptiveThreshConstant = t2;
    RebuildDetector();
#else
    detectorParams->adaptiveThreshConstant = t2;
#endif
}

void Aruco::SetMaskShape(int v) { this->maskShape = v; }
void Aruco::SetMaskRadius(int v) { this->maskRadius = v; }
void Aruco::SetMaskPerspectiveShift(int v) { this->maskPerspectiveShift = v; }
void Aruco::SetMaskValue(int v) { this->maskValue = v; }

void Aruco::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
        active = (int)fn["Active"];
        minSize = (float)fn["MinSize"];
        maxSize = (float)fn["MaxSize"];
        thresh1 = (int)(float)fn["Threshold1"];
        thresh2 = (int)(float)fn["Threshold2"];
        maskShape = (int)fn["MaskShape"];
        maskRadius = (int)fn["MaskRadius"];
        maskPerspectiveShift = (int)fn["MaskPerspectiveShift"];
        maskValue = (int)fn["MaskValue"];
        output = (int)fn["Output"];
        outputFilename = (string)fn["OutputFilename"];
    }
}

void Aruco::SaveXML(FileStorage& fs)
{
    fs << "Active" << active;
    fs << "MinSize" << minSize;
    fs << "MaxSize" << maxSize;
    fs << "Threshold1" << thresh1;
    fs << "Threshold2" << thresh2;
    fs << "MaskShape" << maskShape;
    fs << "MaskRadius" << maskRadius;
    fs << "MaskPerspectiveShift" << maskPerspectiveShift;
    fs << "MaskValue" << maskValue;
    fs << "Output" << output;
    fs << "OutputFilename" << outputFilename;
}

void Aruco::OutputStep()
{
    if (outputStream.is_open())
    {
        for (unsigned int b = 0; b < pipeline->parent->blobs.size(); b++)
        {
            outputStream
                << pipeline->parent->GetPresentTime() << "\t"
                << pipeline->parent->GetPresentFrameNumber() << "\t"
                << pipeline->parent->blobs[b].tagId << "\t"
                << pipeline->parent->blobs[b].x << "\t"
                << pipeline->parent->blobs[b].y << "\t"
                << pipeline->parent->blobs[b].angle << "\t"
                << pipeline->parent->blobs[b].size << "\t"
                << pipeline->parent->blobs[b].zone
                << std::endl;
        }
    }
}

void Aruco::CloseOutput()
{
    if (outputStream.is_open()) outputStream.close();
}

void Aruco::OpenOutput()
{
    if (outputStream.is_open()) outputStream.close();

    if (!outputFilename.empty())
    {
        outputStream.open(outputFilename.c_str(), std::ios::out);
        if (outputStream.is_open())
        {
            outputStream << "time \t frame \t id \t x \t y \t angle \t size \t zone" << std::endl;
        }
    }
}
