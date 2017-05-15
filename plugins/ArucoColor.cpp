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
#include "Blob.h"

#include <iostream>
#include <iomanip>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <string>

using namespace cv;
using namespace std;

//    static const int dictMarkerIDs[] = {5880, 5601, 5261, 2411, 1749, 191, 6431, 1530, 3165, 4986, 3597, 4387, 3522, 1069, 316, 5661, 5276, 3843, 2584, 3380, 5815, 6337, 3473, 1345, 6158, 5809, 4024, 5350, 905, 2147, 1401, 4165, 676, 5306, 3135, 3814, 5938, 4858, 3393, 1212, 1142, 2626, 4401, 5334, 6111, 294, 4792, 254, 4678, 3859, 4534, 4636, 481, 3026, 2154, 3021, 3660, 5410, 5773, 6007, 5777, 4590, 1600, 1920, 3808};

    // int referenceHues[] = {17,  28,  42,  65,  94, 107, 120, 139, 175}; // original !!!

//    static const int referenceHues[] = {19, 28, 47, 75, 96, 105, 113, 135, 173}; // from histograms...

ArucoColor::ArucoColor () : PipelinePlugin()
{
    // // specify markers
    // libac.setMarkersDimensions(2,2);

    // // set hues
    // vector<int> refHues;
    // for (int i = 0; i < 9; i++)
    // 	refHues.push_back(referenceHues[i]);
    // libac.setReferenceHues(refHues);
    
    // // set dictionary
    // vector<int> dict;
    // for (int i = 0; i < 65; i++)
    // 	dict.push_back(dictMarkerIDs[i]);
    // libac.setDictionary(dict);

}

void ArucoColor::Reset()
{
//    multithreaded = true;
}

void ArucoColor::Apply()
{
    // find markers
    vector<ac::ArucoColor::DetectedMarker> detectedMarkers = libac.detect(pipeline->frame);

    libac.segmented.copyTo(pipeline->marked);


    // clean blobs list
    vector<Blob>& blobs = pipeline->parent->blobs;
    blobs.clear();

    // and fill the list with new data
    for (auto& m : detectedMarkers)
    {
	// create a blob
	Blob b (0, 0, 0);
	Point2f pos = m.center;
	b.tagId = m.id;
	b.x = pos.x;
	b.y = pos.y;
	b.size = m.area;
	Point2f dir = m.corners[1] - m.corners[0];
	b.angle = atan2(dir.y, dir.x);

	unsigned int idx = b.x + b.y * pipeline->width;
	b.zone = pipeline->zoneMap.data[idx];

	blobs.push_back(b);
    }    
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


void ArucoColor::SetMarkerCols(int v)
{
    libac.setMarkerCols(v);
}

void ArucoColor::SetMarkerRows(int v)
{
    libac.setMarkerRows(v);
}

void ArucoColor::SetSaturationThreshold(int v)
{
    libac.setSaturationThreshold(v);
}

void ArucoColor::SetValueThreshold(int v)
{
    libac.setValueThreshold(v);
}

void ArucoColor::SetAdaptiveThresholdBlockSize(int v)
{
    libac.setAdaptiveThresholdBlockSize(v);
}

void ArucoColor::SetAdaptiveThresholdConstant(int v)
{
    libac.setAdaptiveThresholdConstant(v);
}

void ArucoColor::SetMinMarkerArea(int v)
{
    libac.setMinMarkerArea(v);
}

void ArucoColor::SetMaxMarkerArea(int v)
{
    libac.setMaxMarkerArea(v);
}

void ArucoColor::SetMaxHueDeviation(int v)
{
    libac.setMaxHueDeviation(v);
}

void ArucoColor::SetMaxMarkerRange(int v)
{
    libac.setMaxMarkerRange(v);
}

void ArucoColor::SetDictionaryString(string str)
{
    std::stringstream stream(str);
    vector<int> d;
    
    int n;
    while (stream.good())
    {	
	if (isdigit(stream.peek()))
	{
	    stream >> n;
	    d.push_back(n);
	}
	else
	{
	    stream.ignore();
	}
    }
    
    libac.setDictionary(d);
}

void ArucoColor::SetReferenceHuesString(string str)
{
    std::stringstream stream(str);
    vector<int> d;
    
    int n;
    while (stream.good())
    {	
	if (isdigit(stream.peek()))
	{
	    stream >> n;
	    d.push_back(n);
	}
	else
	{
	    stream.ignore();
	}
    }
    
    libac.setReferenceHues(d);
}


int ArucoColor::GetMarkerCols()
{
    return libac.getMarkerCols();
}

int ArucoColor::GetMarkerRows()
{
    return libac.getMarkerRows();
}

int ArucoColor::GetSaturationThreshold()
{
    return libac.getSaturationThreshold();
}

int ArucoColor::GetValueThreshold()
{
    return libac.getValueThreshold();
}

int ArucoColor::GetAdaptiveThresholdBlockSize()
{
    return libac.getAdaptiveThresholdBlockSize();    
}

int ArucoColor::GetAdaptiveThresholdConstant()
{
    return libac.getAdaptiveThresholdConstant();    
}

int ArucoColor::GetMinMarkerArea()
{
    return libac.getMinMarkerArea();    
}

int ArucoColor::GetMaxMarkerArea()
{
    return libac.getMaxMarkerArea();
}

int ArucoColor::GetMaxHueDeviation()
{
    return libac.getMaxHueDeviation();  
}

int ArucoColor::GetMaxMarkerRange()
{
    return libac.getMaxMarkerRange();    
}

string ArucoColor::GetDictionaryString()
{
    std::vector<std::pair<int,int>> ci = libac.getDictionary();    
       
    vector<int> codes;
    for (auto p : ci)
	codes.push_back(p.first());
    
    // build a string from marker codes
    stringstream ss;
    copy( codes.begin(), codes.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    
    return s;
}

string ArucoColor::GetReferenceHuesString()
{
    std::vector<int> referenceHues = libac.getReferenceHues();

    // build a string
    stringstream ss;
    copy( referenceHues.begin(), referenceHues.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    
    return s;
}


void ArucoColor::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];

	int v;
	v = (int)fn["MarkerCols"];
	libac.setMarkerCols(v);
	
	v = (int)fn["MarkerRows"];
	libac.setMarkerRows(v);
	
	v = (int)fn["SaturationThreshold"];
	libac.setSaturationThreshold(v);
	
	v = (int)fn["ValueThreshold"];
	libac.setValueThreshold(v);
	
	v = (int)fn["AdaptiveThresholdBlockSize"];
	libac.setAdaptiveThresholdBlockSize(v);
	
	v = (int)fn["AdaptiveThresholdConstant"];
	libac.setAdaptiveThresholdConstant(v);
	
	v = (int)fn["MinMarkerArea"];
	libac.setMinMarkerArea(v);
	
	v = (int)fn["MaxMarkerArea"];
	libac.setMaxMarkerArea(v);
	
	v = (int)fn["MaxHueDeviation"];
	libac.setMaxHueDeviation(v);
	
	v = (int)fn["MaxMarkerRange"];
	libac.setMaxMarkerRange(v);
	
	std::string str;
	fn["ReferenceHues"] >> str;
	SetReferenceHuesString(str);
	
	fn["Dictionary"] >> str;
	SetDictionaryString(str);
		
	output = (int)fn["Output"];
	outputFilename = (string)fn["OutputFilename"];
    }
}

void ArucoColor::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;

    fs << "MarkerCols" << libac.getMarkerCols();
    fs << "MarkerRows" << libac.getMarkerRows();
    fs << "SaturationThreshold" << libac.getSaturationThreshold();
    fs << "ValueThreshold" << libac.getValueThreshold();
    fs << "AdaptiveThresholdBlockSize" << libac.getAdaptiveThresholdBlockSize();
    fs << "AdaptiveThresholdConstant" << libac.getAdaptiveThresholdConstant();
    fs << "MinMarkerArea" << libac.getMinMarkerArea();
    fs << "MaxMarkerArea" << libac.getMaxMarkerArea();
    fs << "MaxHueDeviation" << libac.getMaxHueDeviation();
    fs << "MaxMarkerRange" << libac.getMaxMarkerRange();
    fs << "ReferenceHues" << GetReferenceHuesString();
    fs << "Dictionary" << GetDictionaryString();
    fs << "Output" << output;
    fs << "OutputFilename" << outputFilename;
}

void ArucoColor::OutputStep()
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

void ArucoColor::CloseOutput()
{
    if (outputStream.is_open()) outputStream.close();
}

void ArucoColor::OpenOutput()
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
