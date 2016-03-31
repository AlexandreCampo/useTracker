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

#include "ImageProcessingEngine.h"

#include <GL/glut.h>
#include <iostream>


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <libgen.h>

#include <algorithm>
#include <vector>
#include <queue>
#include <list>
#include <iostream>
#include <thread>

using namespace std;

#ifdef VIMBA
#include <VimbaCPP/Include/VimbaCPP.h>
#include "vimba/ApiController.h"
#endif // VIMBA

#include "Parameters.h"
#include "Utils.h"
#include "Background.h"
#include "Pipeline.h"
#include "PipelinePlugin.h"
#include "Blob.h"

#include "CaptureVideo.h"
#include "CaptureImage.h"
#include "CaptureUSBCamera.h"
#include "CaptureDefault.h"
#ifdef VIMBA
#include "CaptureAVTCamera.h"
#endif // VIMBA


ImageProcessingEngine::ImageProcessingEngine ()
{
}

ImageProcessingEngine::~ImageProcessingEngine ()
{
    // kill all existing threads and delete associated mutexes
    threadsStop = true;
    for (unsigned int i = 0; i < threads.size(); i++)
    {
	threadsStart[i]->unlock();
	threads[i].join();
	delete threadsStart[i];
	delete threadsDone[i];
	delete threadsPause[i];
	delete threadsRestart[i];
    }
    threads.clear();

    // destroy pipelines
    pipelines.clear();
}

void ImageProcessingEngine::Reset(Parameters& parameters)
{
    // no capture is a fatal error...
    if (capture == nullptr)
    {
	cerr << "Fatal error: no capture source defined. Exiting." << endl;
	exit (-1);
    }

    // get background pic
    if (parameters.bgFilename.empty() && bgRecalculate && capture->type != Capture::IMAGE)
    {
	if (bgCalcType == BG_MEDIAN)
	    background = CalculateBackgroundMedian (capture, bgStartTime, bgEndTime, bgFrames);
	else if (bgCalcType == BG_MAX)
	    background = CalculateBackgroundMax (capture, bgStartTime, bgEndTime, bgFrames);
	else if (bgCalcType == BG_MEAN)
	    background = CalculateBackgroundMean (capture, bgStartTime, bgEndTime, bgFrames);
	else
	{
	    cerr << "Unknown background calculation type..." << std::endl;
	    background = Mat::zeros(capture->height, capture->width, CV_8UC3);
	}
    }
    else if (!bgFilename.empty() || !parameters.bgFilename.empty())
    {
	if (!parameters.bgFilename.empty())
	    background = imread (parameters.bgFilename.c_str());
	else
	    background = imread (bgFilename.c_str());


	if (background.cols != capture->width || background.rows != capture->height)
	{
	    cerr << "Background and video dimensions mismatch... please update your background" << std::endl;
	    background = Mat::zeros(capture->height, capture->width, CV_8UC3);
	}
    }
    else
    {
	background = Mat::zeros(capture->height, capture->width, CV_8UC3);
    }

    // if provided, load png of mask
    if (!zonesFilename.empty() || !parameters.zonesFilename.empty())
    {
	if (!parameters.zonesFilename.empty())
	    zoneMap = imread (parameters.zonesFilename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
	else
	    zoneMap = imread (zonesFilename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

	if (zoneMap.cols != capture->width || zoneMap.rows != capture->height)
	{
	    cerr << "Zones mask image and video dimensions mismatch... please update your mask" << std::endl;
	    zoneMap = Mat::ones(capture->height, capture->width, CV_8U);
	}
    }
    // no mask provided, generate a blank one
    else
    {
	zoneMap = Mat::ones(capture->height, capture->width, CV_8U);
    }

    // pipeline data
    marked.create (capture->height, capture->width, CV_8U);
    labels.create (capture->height, capture->width, CV_32S);

    pipelineSnapshot.create (capture->height, capture->width, CV_8U);
    pipelineSnapshotMarked.create (capture->height, capture->width, CV_8U);
    takeSnapshot = false;
    snapshotPos = 0;

    if (threadsCount == 0)
	threadsCount = std::thread::hardware_concurrency();

    SetupThreads();

    std::cerr << "Tracker core has been reset and is ready" << std::endl;
}

void ImageProcessingEngine::Reset()
{
    // no capture is a fatal error...
    if (capture == nullptr)
    {
	cerr << "Fatal error: no capture source defined. Exiting." << endl;
	exit (-1);
    }

    // get background pic
    if (background.cols != capture->width || background.rows != capture->height)
    {
	cerr << "Background and video dimensions mismatch... please update your background" << std::endl;
	background = Mat::zeros(capture->height, capture->width, CV_8UC3);
    }

    // if provided, load png of mask
    if (zoneMap.cols != capture->width || zoneMap.rows != capture->height)
    {
	cerr << "Zones mask image and video dimensions mismatch... please update your mask" << std::endl;
	zoneMap = Mat::ones(capture->height, capture->width, CV_8U);
    }

    // pipeline data
    marked.create (capture->height, capture->width, CV_8U);
    labels.create (capture->height, capture->width, CV_32S);

    pipelineSnapshot.create (capture->height, capture->width, CV_8U);
    pipelineSnapshotMarked.create (capture->height, capture->width, CV_8U);

    if (threadsCount == 0)
	threadsCount = std::thread::hardware_concurrency();

    // reset pipelines
    int sliceHeight = capture->height / threadsCount;
    int y = 0;
    for (unsigned int i = 0; i < threadsCount; i++)
    {
	if (i == threadsCount - 1) sliceHeight = capture->height - y;
	pipelines[i].Reset(Rect(0, y, capture->width, sliceHeight));
	y += sliceHeight;
    }
    pipelines[threadsCount].Reset(Rect(0, 0, capture->width, capture->height));

    std::cerr << "Tracker core has been reset and is ready" << std::endl;
}



void ImageProcessingEngine::LoadXML(FileNode& fn)
{
//     // reset capture
//     if (capture) delete capture;
//     capture = nullptr;

//     // priority to cmdline params
//     if (!parameters.inputVideoFilename.empty())
//     {
// 	capture = new CaptureVideo (parameters.inputVideoFilename);
//     }
//     else if (parameters.usbDevice >= 0)
//     {
// 	capture = new CaptureUSBCamera (parameters.usbDevice);
//     }
//     else if (parameters.avtDevice >= 0)
//     {
// #ifdef VIMBA
// 	capture = new CaptureAVTCamera (parameters.avtDevice);
// 	#endif //VIMBA
//     }

     // read xml file
     if (!fn.empty())
     {
// 	if (!capture)
// 	{
// 	    FileNode fn2 = fn["Source"];
// 	    if (!fn2.empty())
// 	    {
// 		string type = (string)fn2["Type"];
// 		if (type == "video")
// 		{
// 		    capture = new CaptureVideo(fn2);
// 		}
// 		else if (type == "USBcamera")
// 		{
// 		capture = new CaptureUSBCamera(fn2);
// 		}
// 		else if (type == "image")
// 		{
// 		    capture = new CaptureImage(fn2);
// 		}
// #ifdef VIMBA
// 		else if (type == "AVTcamera")
// 		{
// 		    capture = new CaptureAVTCamera(fn2);
// 		}
// #endif // VIMBA
// 	    }
// 	}

	threadsCount = (int)fn["Threads"];
	startTime = (float)fn["StartTime"];
	durationTime = (float)fn["DurationTime"];
	timestep = (float)fn["Timestep"];
	useTimeBoundaries = (int)fn["UseTimeBounds"];
	bgFilename = (string)fn["BackgroundFilename"];
	bgRecalculate = (int)fn["BackgroundRecalculate"];
	bgFrames = (int)fn["BackgroundFrames"];
	bgEndTime = (int)fn["BackgroundEndTime"];
	bgStartTime = (int)fn["BackgroundStartTime"];

	string bt = (string)fn["BackgroundCalcType"];
	if (bt == "median") bgCalcType = BG_MEDIAN;
	else if (bt == "mean") bgCalcType = BG_MEAN;
	else if (bt == "max") bgCalcType = BG_MAX;
	zonesFilename = (string)fn["ZonesFilename"];
    }
}

void ImageProcessingEngine::SaveXML(FileStorage& fs)
{
//    capture->SaveXML(fs);

//    fs << "Threads" << (int)threadsCount;
    fs << "StartTime" << startTime;
    fs << "DurationTime" << durationTime;
    fs << "Timestep" << timestep;
    fs << "UseTimeBounds" << useTimeBoundaries;
    fs << "BackgroundFilename" << bgFilename;
    fs << "BackgroundRecalculate" << bgRecalculate;
    fs << "BackgroundFrames" << bgFrames;
    fs << "BackgroundStartTime" << bgStartTime;
    fs << "BackgroundEndTime" << bgEndTime;
    if (bgCalcType == BG_MEDIAN)
	fs << "BackgroundCalcType" << "median";
    else if (bgCalcType == BG_MEAN)
	fs << "BackgroundCalcType" << "mean";
    else if (bgCalcType == BG_MAX)
	fs << "BackgroundCalcType" << "max";
    fs << "ZonesFilename" << zonesFilename;
}

void ImageProcessingEngine::PushBack (vector<PipelinePlugin*> pfv, bool reset)
{
    // pfv is complete, do a raw copy
    if (pfv.size() == threadsCount + 1)
    {
	for (unsigned int i = 0; i <= threadsCount; i++)
	{
	    pipelines[i].plugins.push_back(pfv[i]);
	    if (reset)
	    {
		if (pfv[i])
		{
		    pfv[i]->pipeline = &pipelines[i];
		    pfv[i]->Reset();
		}
	    }
	}
    }

    // multithreaded, pfv contains plugin ptrs for multithreading, not for single thread
    else if (pfv.size() == threadsCount)
    {
	for (unsigned int i = 0; i < threadsCount; i++)
	{
	    pipelines[i].plugins.push_back(pfv[i]);
	    if (reset)
	    {
		pfv[i]->pipeline = &pipelines[i];
		pfv[i]->Reset();
	    }
	}
	pipelines[threadsCount].plugins.push_back(nullptr);
    }
    // single thread, pfv contains a single plugin ptr
    else
    {
	for (unsigned int i = 0; i < threadsCount; i++)
	    pipelines[i].plugins.push_back(nullptr);

	pipelines[threadsCount].plugins.push_back(pfv[0]);
	if (reset)
	{
	    pfv[0]->pipeline = &pipelines[threadsCount];
	    pfv[0]->Reset();
	}
    }
}

void ImageProcessingEngine::Insert (int pos, vector<PipelinePlugin*> pfv, bool reset)
{
    // pfv is complete, do a raw copy
    if (pfv.size() == threadsCount + 1)
    {
	for (unsigned int i = 0; i <= threadsCount; i++)
	{
	    pipelines[i].plugins.insert(pipelines[i].plugins.begin()+pos, pfv[i]);
	    if (reset)
	    {
		if (pfv[i])
		{
		    pfv[i]->pipeline = &pipelines[i];
		    pfv[i]->Reset();
		}
	    }
	}
    }
    // multithreaded, pfv contains plugin ptrs for multithreading, not for single thread
    else if (pfv.size() == threadsCount)
    {
	for (unsigned int i = 0; i < threadsCount; i++)
	{
	    pipelines[i].plugins.insert(pipelines[i].plugins.begin()+pos, pfv[i]);
	    if (reset)
	    {
		pfv[i]->pipeline = &pipelines[i];
		pfv[i]->Reset();
	    }
	}
	pipelines[threadsCount].plugins.insert(pipelines[threadsCount].plugins.begin()+pos, nullptr);
    }
    // single thread, pfv contains a single plugin ptr
    else
    {
	for (unsigned int i = 0; i < threadsCount; i++)
	    pipelines[i].plugins.insert(pipelines[i].plugins.begin()+pos, nullptr);

	pipelines[threadsCount].plugins.insert(pipelines[threadsCount].plugins.begin()+pos, pfv[0]);
	if (reset)
	{
	    pfv[0]->pipeline = &pipelines[threadsCount];
	    pfv[0]->Reset();
	}
    }
}

vector<PipelinePlugin*> ImageProcessingEngine::Erase (unsigned int pos)
{
    vector<PipelinePlugin*> pfv;
    vector<PipelinePlugin*>::iterator it;
    for (unsigned int i = 0; i <= threadsCount; i++)
    {
	it = pipelines[i].plugins.begin() + pos;
	pfv.push_back(*it);
	pipelines[i].plugins.erase(it);
    }
    return pfv;
}

void ImageProcessingEngine::SetupThreads ()
{
    // kill all existing threads and delete associated mutexes
    threadsStop = true;
    for (unsigned int i = 0; i < threads.size(); i++)
    {
	threadsStart[i]->unlock();
	threads[i].join();
	delete threadsStart[i];
	delete threadsDone[i];
	delete threadsPause[i];
	delete threadsRestart[i];
    }
    threads.clear();
    threadsStart.clear();
    threadsDone.clear();
    threadsPause.clear();
    threadsRestart.clear();
    threadsStop = false;

    // destroy pipelines
    pipelines.clear();

    // create new pipelines
    int sliceHeight = capture->height / threadsCount;
    int y = 0;
    for (unsigned int i = 0; i < threadsCount; i++)
    {
	if (i == threadsCount - 1) sliceHeight = capture->height - y;
	pipelines.push_back(Pipeline(this, Rect(0, y, capture->width, sliceHeight)));
	y += sliceHeight;
    }
    // this is the pipeline for the non multithreaded plugins
    pipelines.push_back(Pipeline(this, Rect(0, 0, capture->width, capture->height)));

    // spawn new threads, including special thread, also allocate new mutexes
    for (unsigned int i = 0; i <= threadsCount; i++)
    {
	threadsStart.push_back(new mutex());
	threadsDone.push_back(new mutex());
	threadsPause.push_back(new mutex());
	threadsRestart.push_back(new mutex());

	threadsStart[i]->lock();
	threadsDone[i]->lock();
	threadsPause[i]->lock();
	threadsRestart[i]->lock();

	threads.push_back(thread(&ImageProcessingEngine::PipelineThread, this, i));
    }
}

void ImageProcessingEngine::PipelineThread (unsigned int p)
{
    cout << "pipeline thread " << p << " is launched" << std::endl;

    // keep running
    while (1)
    {
	// wait for signal to run
	threadsStart[p]->lock();

	// end the thread ?
	if (threadsStop) return;

	// do the task
	// process plugins in the pipeline
	if (p != threadsCount)
	{
	    for (unsigned int i = 0; i < pipelines[p].plugins.size(); i++)
	    {
		if (pipelines[p].plugins[i])
		{
		    if (pipelines[p].plugins[i]->active)
		    {
			pipelines[p].plugins[i]->Apply();

			if (output
			    && pipelines[p].plugins[i]->output
			    && !staticFrame)
			{
			    pipelines[p].plugins[i]->OutputStep();
			}
		    }

		    if (takeSnapshot && snapshotPos == i)
		    {
			pipelines[p].marked.copyTo(pipelines[p].pipelineSnapshotMarked);
			if (threadsDrawHud) pipelines[p].plugins[i]->OutputHud(hud);
		    }
		}
		// the special thread will take care of this plugin, wait...
		else
		{
		    threadsPause[p]->unlock();
		    threadsRestart[p]->lock();
		}
	    }
	}
	// code for the special thread
	else
	{
	    for (unsigned int i = 0; i < pipelines[p].plugins.size(); i++)
	    {
		if (pipelines[p].plugins[i])
		{
		    // sync with other threads until they all wait
		    for (unsigned int j = 0; j < threadsCount; j++) threadsPause[j]->lock();

		    if (pipelines[p].plugins[i]->active || (takeSnapshot && snapshotPos == i))
		    {
			if (pipelines[p].plugins[i]->active)
			{
			    pipelines[p].plugins[i]->Apply();

			    if (output
				&& pipelines[p].plugins[i]->output
				&& !staticFrame)
			    {
			    	pipelines[p].plugins[i]->OutputStep();
			    }
			}

			if (takeSnapshot && snapshotPos == i)
			{
			    pipelines[p].marked.copyTo(pipelines[p].pipelineSnapshotMarked);
			    if (threadsDrawHud) pipelines[p].plugins[i]->OutputHud(hud);
			}
		    }

		    // end of operation, free all threads
		    for (unsigned int j = 0; j < threadsCount; j++) threadsRestart[j]->unlock();
		}
	    }
	}

	// advertise end of operation, ready for the next frame
	threadsDone[p]->unlock();
    }
}

void ImageProcessingEngine::CloseOutput ()
{
    for (unsigned int p = 0; p < pipelines.size(); p++)
    {
	for (unsigned int i = 0; i < pipelines[p].plugins.size(); i++)
	{
	    if (pipelines[p].plugins[i])
	    {
		if (pipelines[p].plugins[i]->active)
		{
		    pipelines[p].plugins[i]->CloseOutput();
		}
	    }
	}
    }
    output = false;
}

void ImageProcessingEngine::OpenOutput ()
{
    for (unsigned int p = 0; p < pipelines.size(); p++)
    {
	for (unsigned int i = 0; i < pipelines[p].plugins.size(); i++)
	{
	    if (pipelines[p].plugins[i])
	    {
		if (pipelines[p].plugins[i]->active)
		{
		    if (pipelines[p].plugins[i]->output)
		    {
			pipelines[p].plugins[i]->OpenOutput();
		    }
		}
	    }
	}
    }
    output = true;
}

void ImageProcessingEngine::Step(bool drawHud)
{
    bool forceStep = false;

    double ctime = capture->GetTime();

    // first check the status of the capture
    if (capture->statusChanged)
    {
	if (capture->isPaused || capture->isStopped)
	{
	    forceStep = true;
	}
	// play was pressed
	else
	{
	    nextStepTime = ctime;
	}

	// reset the flag
	capture->statusChanged = false;
    }

    if (!forceStep)
    {
	// check time boundaries
	if (useTimeBoundaries)
	{
	    if (ctime < startTime)
		return;
	    if (durationTime > 0.0000001 && ctime > (startTime + durationTime))
		return;
	}

	// finally respect timestep if it is set
	if (timestep > 0.00001 && ctime < nextStepTime)
	    return;
    }

    // passed all tests, proceed to image analysis
    nextStepTime += timestep;

    threadsDrawHud = drawHud;
    hud.setTo (0);

    // check if frame is static
    lastFrameNumber = frameNumber;
    frameNumber = capture->GetFrameNumber();
    staticFrame = (frameNumber == lastFrameNumber);

    // prepare marked buffer
    marked.setTo(255);

    // call the threads
    for (unsigned int i = 0; i <= threadsCount; i++)
    {
	threadsStart[i]->unlock();
    }

    // wait for end of their operation
    for (unsigned int i = 0; i <= threadsCount; i++)
    {
	threadsDone[i]->lock();
    }

    // take snapshot if needed
    if (takeSnapshot)
    {
	threshold(pipelineSnapshotMarked, pipelineSnapshot, 0, 255, THRESH_BINARY);
    }
}

bool ImageProcessingEngine::GetNextFrame()
{
    bool capres = capture->GetNextFrame();
    double ctime = capture->GetTime();

    // respect time bounds (not implementing forward jump...)
    if (useTimeBoundaries && durationTime > 0.0000001)
    	if (ctime > (startTime + durationTime))
    	    return false;

    return capres;
}

