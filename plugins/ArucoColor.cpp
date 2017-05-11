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

#include "ArucoColor.h"

#include "ImageProcessingEngine.h"

#include <iostream>
#include <iomanip>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <string>

using namespace cv;
using namespace std;

    static const int dictMarkerIDs[] = {5880, 5601, 5261, 2411, 1749, 191, 6431, 1530, 3165, 4986, 3597, 4387, 3522, 1069, 316, 5661, 5276, 3843, 2584, 3380, 5815, 6337, 3473, 1345, 6158, 5809, 4024, 5350, 905, 2147, 1401, 4165, 676, 5306, 3135, 3814, 5938, 4858, 3393, 1212, 1142, 2626, 4401, 5334, 6111, 294, 4792, 254, 4678, 3859, 4534, 4636, 481, 3026, 2154, 3021, 3660, 5410, 5773, 6007, 5777, 4590, 1600, 1920, 3808};

    // int referenceHues[] = {17,  28,  42,  65,  94, 107, 120, 139, 175}; // original !!!

    static const int referenceHues[] = {19, 28, 47, 75, 96, 105, 113, 135, 173}; // from histograms...

ArucoColor::ArucoColor () : PipelinePlugin()
{

    // specify markers
    libac.setMarkersDimensions(2,2);

    // set hues
    vector<int> refHues;
    for (int i = 0; i < 9; i++)
	refHues.push_back(referenceHues[i]);
    libac.setReferenceHues(refHues);
    
    // set dictionary
    vector<int> dict;
    for (int i = 0; i < 65; i++)
	dict.push_back(dictMarkerIDs[i]);
    libac.setDictionary(dict);

}

void ArucoColor::Reset()
{
//    multithreaded = true;
    SetSize(size);
    result = Mat::zeros (pipeline->height, pipeline->width, CV_8U);
}


void ArucoColor::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	size = (int)fn["Size"];
    }
}

void ArucoColor::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Size" << size;
}

void ArucoColor::SetSize (int s)
{
    size = s;

    structuringElement = getStructuringElement(
	MORPH_ELLIPSE,
	Size (2 * size + 1, 2 * size + 1),
	Point (size, size)
	);
}




void ArucoColor::Apply()
{
    // find markers
    vector<ac::ArucoColor::DetectedMarker> detectedMarkers = libac.detect(pipeline->frame);

    libac.segmented.copyTo(pipeline->marked);

}

void ArucoColor::OutputHud (Mat& hud)
{
    // output 
    libac.drawMarkers(hud);

    
    // for (unsigned int i = 0; i < markers.size(); i++)
    // {
    // 	markers[i].draw(hud, Scalar(0,255,0), 2);
    // }
}
