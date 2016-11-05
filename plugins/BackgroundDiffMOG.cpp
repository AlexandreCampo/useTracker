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

#include "BackgroundDiffMOG.h"


using namespace cv;

BackgroundDiffMOG::BackgroundDiffMOG() : PipelinePlugin()
{
    multithreaded = true;
}

BackgroundDiffMOG::~BackgroundDiffMOG()
{
    delete MOG;
    multithreaded = true;
}

void BackgroundDiffMOG::Reset()
{
    diff = Mat(pipeline->height, pipeline->width, CV_8UC3);
    sum = Mat(pipeline->height, pipeline->width, CV_8U);
    marked2 = Mat(pipeline->height, pipeline->width, CV_8U);
    marked3 = Mat(pipeline->height, pipeline->width, CV_8U);

    delete MOG;
    MOG = new BackgroundSubtractorMOG(history, nMixtures, backgroundRatio, noiseSigma);    
    MOG->operator()(pipeline->background, marked2);	
}


void BackgroundDiffMOG::SetHistory(int h)
{
    history = h;
    
    delete MOG;
    MOG = new BackgroundSubtractorMOG(history, nMixtures, backgroundRatio, noiseSigma);    
    MOG->operator()(pipeline->background, marked2);	
}

void BackgroundDiffMOG::SetNMixtures(int m)
{
    nMixtures = m;
    
    delete MOG;
    MOG = new BackgroundSubtractorMOG(history, nMixtures, backgroundRatio, noiseSigma);    
    MOG->operator()(pipeline->background, marked2);	
}

void BackgroundDiffMOG::SetBackgroundRatio(double r)
{
    backgroundRatio = r;
    
    delete MOG;
    MOG = new BackgroundSubtractorMOG(history, nMixtures, backgroundRatio, noiseSigma);    
    MOG->operator()(pipeline->background, marked2);	
}

void BackgroundDiffMOG::SetNoiseSigma(double s)
{
    noiseSigma = s;
    
    delete MOG;
    MOG = new BackgroundSubtractorMOG(history, nMixtures, backgroundRatio, noiseSigma);    
    MOG->operator()(pipeline->background, marked2);	
}

void BackgroundDiffMOG::Apply()
{    
    MOG->operator()(pipeline->frame, marked2, learningRate);

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

void BackgroundDiffMOG::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	history = (int)fn["History"];
	nMixtures = (int)fn["nMixtures"];
	backgroundRatio = (float)fn["BackgroundRatio"];
	noiseSigma = (float)fn["NoiseSigma"];
	learningRate = (float)fn["LearningRate"];

	restrictToZone = (int)fn["RestrictToZone"];
	if (restrictToZone)
	    zone = (int)fn["Zone"];
	additive = (int)fn["Additive"];
    }
}

void BackgroundDiffMOG::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "History" << history;
    fs << "nMixtures" << nMixtures;
    fs << "BackgroundRatio" << backgroundRatio;
    fs << "NoiseSigma" << noiseSigma;
    fs << "LearningRate" << learningRate;

    fs << "RestrictToZone" << restrictToZone;
    if (restrictToZone)
	fs << "Zone" << zone;
    fs << "Additive" << additive;
}

