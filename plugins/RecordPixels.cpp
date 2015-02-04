
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
			<< pipeline->parent->capture->time << "\t"
			<< pipeline->parent->capture->frameNumber << "\t"
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
