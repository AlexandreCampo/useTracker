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

#include "Watershed.h"


using namespace cv;

Watershed::Watershed() : PipelinePlugin()
{
//    multithreaded = true;
}

Watershed::~Watershed()
{
//    multithreaded = true;
}

void Watershed::Reset()
{
    diff = Mat(pipeline->height, pipeline->width, CV_8UC3);
    sum = Mat(pipeline->height, pipeline->width, CV_8U);
    marked2 = Mat(pipeline->height, pipeline->width, CV_8U);
    marked3 = Mat(pipeline->height, pipeline->width, CV_8U);
}


void Watershed::Apply()
{
    
    absdiff (pipeline->frame, pipeline->background, diff);
    cvtColor(diff, sum, CV_BGR2GRAY);

    // normalize(sum, sum, 0, 255, NORM_MINMAX);
    // imshow("Distance Transform Image", sum);

    // forget that ... cv::threshold(sum, marked2, 0, 255, THRESH_BINARY | THRESH_OTSU);


    cv::threshold(sum, marked2, threshold, 255, THRESH_BINARY);
    
    if (restrictToZone)
    {
	cv::inRange(pipeline->zoneMap, zone, zone, marked3);
	marked2 &= marked3;	    
    }
    
    if (additive)	
	pipeline->marked |= marked2;
    else
	pipeline->marked &= marked2;

    
//    normalize(sum, sum, 0, 255, NORM_MINMAX);
//    imshow("Distance Transform Image", marked2);

    // perform dist transform first
    Mat dist;
    distanceTransform(pipeline->marked, dist, CV_DIST_L2, 3);

    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1.0, NORM_MINMAX);
    imshow("Distance Transform Image", dist);

    
}

void Watershed::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	threshold = (int)fn["Threshold"];
	restrictToZone = (int)fn["RestrictToZone"];
	if (restrictToZone)
	    zone = (int)fn["Zone"];
	additive = (int)fn["Additive"];
    }
}

void Watershed::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Threshold" << threshold;
    fs << "RestrictToZone" << restrictToZone;
    if (restrictToZone)
	fs << "Zone" << zone;
    fs << "Additive" << additive;
    
}

