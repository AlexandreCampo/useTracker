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

#include "FrameDifference.h"


#include "ImageProcessingEngine.h"

using namespace cv;

FrameDifference::FrameDifference () : PipelinePlugin()
{
    multithreaded = true;
}

void FrameDifference::Reset()
{
    previousFrame = Mat::zeros (pipeline->height, pipeline->width, CV_8UC3);
    diffFrame = Mat::zeros (pipeline->height, pipeline->width, CV_8UC3);
    sumFrame = Mat::zeros (pipeline->height, pipeline->width, CV_8U);
    previousMarked = Mat::zeros (pipeline->height, pipeline->width, CV_8UC1);
    currentMarked = Mat::zeros (pipeline->height, pipeline->width, CV_8UC1);

    marked2 = Mat(pipeline->height, pipeline->width, CV_8U);
    marked3 = Mat(pipeline->height, pipeline->width, CV_8U);
}

void FrameDifference::Apply()
{
    if (usePipeline)
    {
        if (!pipeline->parent->staticFrame)
        {
            Mat tmp = pipeline->marked.clone();
            for (int i = 0; i < pipeline->width*pipeline->height; i++)
            {
                pipeline->marked.data[i] = (~previousMarked.data[i]) & pipeline->marked.data[i];
            }
            previousMarked = currentMarked;
            currentMarked = tmp;
        }
        else
        {
            for (int i = 0; i < pipeline->width*pipeline->height; i++)
            {
                pipeline->marked.data[i] = (~previousMarked.data[i]) & pipeline->marked.data[i];
            }
        }
    }
    // difference between source frames
    else
    {
        if (!pipeline->parent->staticFrame)
        {
            absdiff (pipeline->frame, previousFrame, diffFrame);
            cvtColor(diffFrame, sumFrame, COLOR_BGR2GRAY);       
            inRange(sumFrame, threshold, 255, marked2);        
            previousFrame = pipeline->frame.clone();
        }
        else
        {
            inRange(sumFrame, threshold, 255, marked2);        
        }
    }    
    
    if (restrictToZone)
    {
        cv::inRange(pipeline->zoneMap, zone, zone, marked3);
        marked2 &= marked3;	    
    }
    
    if (additive)	
        pipeline->marked |= (marked2 & pipeline->zoneMap);
    else
        pipeline->marked &= marked2;
}

void FrameDifference::SetUsePipeline(bool up)
{
    usePipeline = up;
}

void FrameDifference::SetThreshold(int t)
{
    threshold = t;
}



void FrameDifference::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
        active = (int)fn["Active"];
                
        usePipeline = (int)fn["UsePipeline"];
        threshold = (int)fn["Threshold"];
        
        restrictToZone = (int)fn["RestrictToZone"];
        if (restrictToZone)
            zone = (int)fn["Zone"];
        additive = (int)fn["Additive"];
    }
}

void FrameDifference::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;

    fs << "UsePipeline" << usePipeline;
    fs << "Threshold" << threshold;
    
    fs << "RestrictToZone" << restrictToZone;
    if (restrictToZone)
	fs << "Zone" << zone;
    fs << "Additive" << additive;
}
