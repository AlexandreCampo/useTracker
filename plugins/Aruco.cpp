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

#include "Aruco.h"

#include "Blob.h"
#include "ImageProcessingEngine.h"

using namespace cv;
using namespace std;
using namespace aruco;


Aruco::Aruco () : PipelinePlugin()
{
}

Aruco::~Aruco()
{
    CloseOutput();
}

void Aruco::Reset()
{
    detector.setThresholdParams (thresh1, thresh2);
    detector.setCornerRefinementMethod (MarkerDetector::SUBPIX);
//    detector.setCornerRefinementMethod (MarkerDetector::HARRIS);
    detector.setMinMaxSize (minSize,maxSize);
    detector.setThresholdMethod(thresholdMethod);
}

void Aruco::Apply()
{
    detector.detect(pipeline->frame, markers, cameraParameters, -1);
    detector.getThresholdedImage().copyTo(pipeline->marked);

    // clean blobs list
    vector<Blob>& blobs = pipeline->parent->blobs;
    blobs.clear();

    // and fill the list with new data
    for (auto m : markers)
    {
	// create a blob
	Blob b (0, 0, 0);
	Point2f pos = m.getCenter();
	b.tagId = m.id;
	b.x = pos.x;
	b.y = pos.y;
	b.size = m.getArea();
    Point2f dir = m[1] - m[0];
	b.angle = atan2(dir.y, dir.x);

	// TODO this is dangerous if aruco uses cam model to produce transformed coordinates
	unsigned int idx = b.x + b.y * pipeline->width;
	b.zone = pipeline->zoneMap.data[idx];

	blobs.push_back(b);
    }
}

void Aruco::OutputHud (Mat& hud)
{
    for (unsigned int i = 0; i < markers.size(); i++)
    {
	markers[i].draw(hud, Scalar(0,255,0), 2);
    }
}

void Aruco::SetMinSize(double minSize)
{
    this->minSize = minSize;
    detector.setMinMaxSize(minSize, maxSize);
}

void Aruco::SetMaxSize(double maxSize)
{
    this->maxSize = maxSize;
    detector.setMinMaxSize(minSize, maxSize);
}

void Aruco::SetThreshold1(int t1)
{
    this->thresh1 = t1;
    detector.setThresholdParams((double)thresh1,(double)thresh2);
}

void Aruco::SetThreshold2(int t2)
{
    this->thresh2 = t2;
    detector.setThresholdParams((double)thresh1,(double)thresh2);
}

void Aruco::SetThresholdMethod(int m)
{
    this->thresholdMethod = (aruco::MarkerDetector::ThresholdMethods)m;
    detector.setThresholdMethod(thresholdMethod);
}

void Aruco::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];

	minSize = (float)fn["MinSize"];
	maxSize = (float)fn["MaxSize"];

	thresh1 = (float)fn["Threshold1"];
	thresh2 = (float)fn["Threshold2"];

	string tm = (string)fn["ThresholdMethod"];	
	if (tm == "Fixed")
	    thresholdMethod = aruco::MarkerDetector::FIXED_THRES;
	else if (tm == "Adaptive")
	    thresholdMethod = aruco::MarkerDetector::ADPT_THRES;
	else if (tm == "Canny")
	    thresholdMethod = aruco::MarkerDetector::CANNY;

	output = (int)fn["Output"];
	outputFilename = (string)fn["OutputFilename"];
    }
}

void Aruco::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "MinSize" << minSize;
    fs << "MaxSize" << maxSize;

    if (thresholdMethod == aruco::MarkerDetector::FIXED_THRES)
	fs << "ThresholdMethod" << "Fixed";
    else if (thresholdMethod == aruco::MarkerDetector::ADPT_THRES)
	fs << "ThresholdMethod" << "Adaptive";
    else if (thresholdMethod == aruco::MarkerDetector::CANNY)
	fs << "ThresholdMethod" << "Canny";
	
    fs << "Threshold1" << thresh1;
    fs << "Threshold2" << thresh2;

    fs << "Output" << output;
    fs << "OutputFilename" << outputFilename;
}

void Aruco::OutputStep()
{
    // in any case, also output blob list with characs...
    if (outputStream.is_open())
    {
	for (unsigned int b = 0; b < pipeline->parent->blobs.size(); b++)
	{
	    outputStream
		<< pipeline->parent->capture->GetTime() << "\t"
		<< pipeline->parent->capture->GetFrameNumber() << "\t"
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
    // open normal output
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

#endif
