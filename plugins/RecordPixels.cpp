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

#include "RecordPixels.h"

#include "ImageProcessingEngine.h"

#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

RecordPixels::~RecordPixels()
{
    CloseOutput();
}

void RecordPixels::Reset()
{
//    if (output) OpenOutput();
}

void RecordPixels::Apply()
{
}

void RecordPixels::OutputStep()
{
    if (outputStream.is_open())
    {
	for (int y = 0; y < pipeline->height; y++)
	{
	    unsigned char* row = pipeline->marked.ptr<unsigned char>(y);
	    for (int x = 0; x < pipeline->width; x++)
	    {
		if (row[x] > 0)
		{
		    outputStream
			<< pipeline->parent->capture->GetTime()<< "\t"
			<< pipeline->parent->capture->GetFrameNumber() << "\t"
			<< (int) (pipeline->zoneMap.at<unsigned char>(y,x)) << "\t"
			<< x << "\t"
			<< y
			<< std::endl;
		}
	    }
	}
    }
}

void RecordPixels::CloseOutput()
{
    if (outputStream.is_open()) outputStream.close();
}

void RecordPixels::OpenOutput()
{
    if (outputStream.is_open()) outputStream.close();

    if (!outputFilename.empty())
    {
	outputStream.open(outputFilename.c_str(), std::ios::out);
	if (outputStream.is_open())
	{
	    outputStream << "time \t frame \t zone \t x \t y " << std::endl;
	}
    }
}

void RecordPixels::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	output = (int)fn["Output"];
	outputFilename = (string)fn["OutputFilename"];
   }
}

void RecordPixels::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Output" << output;
    fs << "OutputFilename" << outputFilename;
}
