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

#include "ColorSegmentation.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#include "Pipeline.h"

using namespace cv;
using namespace std;

ColorSegmentation::ColorSegmentation() : PipelinePlugin ()
{
    multithreaded = true;
}

void ColorSegmentation::Reset()
{
    frameHSV = Mat(pipeline->height, pipeline->width, CV_8UC3);
    marked2 = Mat(pipeline->height, pipeline->width, CV_8UC3);

    pixelSrc = Mat (1, 1, CV_8UC3);
    pixelDst = Mat (1, 1, CV_8UC3);
    pixelRes = Mat (1, 1, CV_8UC1);
    pixelRes2 = Mat (1, 1, CV_8UC1);
}

void ColorSegmentation::Apply()
{
    if (type == HSV)
    {
	// convert BGR frame to HSV
	cvtColor (pipeline->frame, frameHSV, CV_BGR2HSV);

	// threshold to retain only desired pixels
	if (minHSV[0] <= maxHSV[0])
	{
	    inRange(frameHSV, minHSV, maxHSV, marked2);
	    pipeline->marked &= marked2;
	}
	else
	{
	    Vec3b tmp = maxHSV;
	    tmp[0] = 180;
	    inRange(frameHSV, minHSV, tmp, marked2);
	    pipeline->marked &= marked2;

	    tmp = minHSV;
	    tmp[0] = 0;
	    inRange(frameHSV, tmp, maxHSV, marked2);
	    pipeline->marked &= marked2;
	}
    }
    else
    {
	// threshold to retain only desired pixels
	inRange(pipeline->frame, minBGR, maxBGR, marked2);
	pipeline->marked &= marked2;
    }
}


void ColorSegmentation::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	string tmp = (string)fn["Type"];

    	if (tmp == "RGB")
	{
	    type = BGR;

	    int minR = 0, minG = 0, minB = 0, maxR = 255, maxG = 255, maxB = 255;
	    minR = (int)fn["minR"];
	    minG = (int)fn["minG"];
	    minB = (int)fn["minB"];
	    maxR = (int)fn["maxR"];
	    maxG = (int)fn["maxG"];
	    maxB = (int)fn["maxB"];

	    minBGR = Vec3b (minB, minG, minR);
	    maxBGR = Vec3b (maxB, maxG, maxR);
	}
	else
	{
	    type = HSV;

	    int minH = 0, minS = 0, minV = 0, maxH = 255, maxS = 255, maxV = 255;
	    minH = (int)fn["minH"];
	    minS = (int)fn["minS"];
	    minV = (int)fn["minV"];
	    maxH = (int)fn["maxH"];
	    maxS = (int)fn["maxS"];
	    maxV = (int)fn["maxV"];

	    minHSV = Vec3b (minH / 2, minS, minV);
	    maxHSV = Vec3b (maxH / 2, maxS, maxV);
	}
    }
}

void ColorSegmentation::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;

    if (type == HSV)
    {
	fs << "Type" << "HSV";

	fs << "minH" << minHSV[0] * 2;
	fs << "minS" << minHSV[1];
	fs << "minV" << minHSV[2];
	fs << "maxH" << maxHSV[0] * 2;
	fs << "maxS" << maxHSV[1];
	fs << "maxV" << maxHSV[2];
    }
    if (type == BGR)
    {
	fs << "Type" << "RGB";

	fs << "minR" << minBGR[2];
	fs << "minG" << minBGR[1];
	fs << "minB" << minBGR[0];
	fs << "maxR" << maxBGR[2];
	fs << "maxG" << maxBGR[1];
	fs << "maxB" << maxBGR[0];
    }
}
