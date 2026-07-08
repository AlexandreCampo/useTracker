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

#include <iostream>

#include <algorithm>
#include <vector>
#include <queue>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

using namespace std;

#include "Parameters.h"
#include "Utils.h"
#include "Background.h"
#include "Pipeline.h"
#include "PipelinePlugin.h"
#include "Blob.h"

#include "CaptureVideo.h"
#include "CaptureMultiVideo.h"
#include "CaptureImage.h"
#include "CaptureUSBCamera.h"
#include "CaptureDefault.h"


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
	{
	    background = CalculateBackgroundMedian (capture, bgStartTime, bgEndTime, bgFrames, bgLowThreshold, bgHighThreshold);
	}
	else if (bgCalcType == BG_MEAN)
	{
	    background = CalculateBackgroundMean (capture, bgStartTime, bgEndTime, bgFrames, bgLowThreshold, bgHighThreshold);
	}
	else
	{
	    cerr << "Unknown background calculation type..." << std::endl;
	    background = cv::Mat::zeros(capture->height, capture->width, CV_8UC3);
	}
    }
    else if (!bgFilename.empty() || !parameters.bgFilename.empty())
    {
	if (!parameters.bgFilename.empty())
	    background = cv::imread (parameters.bgFilename.c_str());
	else
	    background = cv::imread (bgFilename.c_str());


	if (background.cols != capture->width || background.rows != capture->height)
	{
	    cerr << "Background and video dimensions mismatch... please update your background" << std::endl;
	    background = cv::Mat::zeros(capture->height, capture->width, CV_8UC3);
	}
    }
    else
    {
	background = cv::Mat::zeros(capture->height, capture->width, CV_8UC3);
    }

    // if provided, load png of mask
    if (!zonesFilename.empty() || !parameters.zonesFilename.empty())
    {
	if (!parameters.zonesFilename.empty())
	    zoneMap = cv::imread (parameters.zonesFilename.c_str(), cv::IMREAD_GRAYSCALE);
	else
	    zoneMap = cv::imread (zonesFilename.c_str(), cv::IMREAD_GRAYSCALE);

	if (zoneMap.cols != capture->width || zoneMap.rows != capture->height)
	{
	    cerr << "Zones mask image and video dimensions mismatch... please update your mask" << std::endl;
	    zoneMap = cv::Mat::ones(capture->height, capture->width, CV_8U);
	}
    }
    // no mask provided, generate a blank one
    else
    {
	zoneMap = cv::Mat::ones(capture->height, capture->width, CV_8U);
    }

    takeSnapshot = false;
    snapshotPos = 0;

    if (threadsCount == 0)
	threadsCount = std::thread::hardware_concurrency();

    // stash the source-resolution background/zone and allocate the shared
    // buffers at the (possibly scaled) processing resolution BEFORE creating
    // the pipelines, since the Pipeline constructor slices those buffers
    CaptureNativeBgZone();
    BuildProcBuffers();

    SetupThreads();

    // force start time and duration if requested from command line
    if (parameters.startTime >= -0.000001)
    {
	startTime = parameters.startTime;
	useTimeBoundaries = true;
    }
    if (parameters.durationTime >= -0.000001)
    {
	durationTime = parameters.durationTime;
	useTimeBoundaries = true;
    }

    // just in case...
    if (startTime < 0) startTime = 0;

    // if specific start time was asked, seek there
    if (parameters.startTime >= 0 || useTimeBoundaries)
    {
	if(capture->type == Capture::VIDEO)
	{
	    CaptureVideo* capv = dynamic_cast<CaptureVideo*>(capture);
	    if (capv) capv->SetTime(startTime);
	}
	if(capture->type == Capture::MULTI_VIDEO)
	{
	    CaptureMultiVideo* capv = dynamic_cast<CaptureMultiVideo*>(capture);
	    if (capv) capv->SetTime(startTime);
	}
    }

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

    if (threadsCount == 0)
	threadsCount = std::thread::hardware_concurrency();

    // a runtime background/zone image just assigned by the GUI is at source
    // resolution; stash it, then rebuild all buffers/slices at proc resolution
    CaptureNativeBgZone();
    BuildProcBuffers();

    std::cerr << "Tracker core has been reset and is ready" << std::endl;
}

// remember the current playback size in procWidth/procHeight
void ImageProcessingEngine::UpdateProcSize()
{
    if (!capture) { procWidth = procHeight = 0; return; }
    if (inputScale > 0.999f)
    {
	procWidth  = capture->width;
	procHeight = capture->height;
	return;
    }
    procWidth  = std::max(16, (int)(capture->width  * (double)inputScale + 0.5));
    procHeight = std::max(16, (int)(capture->height * (double)inputScale + 0.5));
}

// keep a source-resolution copy of a background/zone image (only when it is
// actually at source size, so a scaled buffer is never mistaken for native)
void ImageProcessingEngine::CaptureNativeBgZone()
{
    if (!capture) return;
    cv::Size nativeSz(capture->width, capture->height);
    if (!background.empty() && background.size() == nativeSz)
	backgroundNative = background.clone();
    if (!zoneMap.empty() && zoneMap.size() == nativeSz)
	zoneMapNative = zoneMap.clone();
}

// (re)allocate the shared buffers and re-slice the pipelines at proc resolution
void ImageProcessingEngine::BuildProcBuffers()
{
    UpdateProcSize();
    cv::Size ps(procWidth, procHeight);

    // background / zone map derived from the source-resolution originals
    if (!backgroundNative.empty())
    {
	if (backgroundNative.size() == ps) background = backgroundNative.clone();
	else cv::resize(backgroundNative, background, ps, 0, 0, cv::INTER_AREA);
    }
    else background = cv::Mat::zeros(ps, CV_8UC3);

    if (!zoneMapNative.empty())
    {
	if (zoneMapNative.size() == ps) zoneMap = zoneMapNative.clone();
	else cv::resize(zoneMapNative, zoneMap, ps, 0, 0, cv::INTER_NEAREST);
    }
    else zoneMap = cv::Mat::ones(ps, CV_8U);

    marked.create(ps, CV_8U);
    labels.create(ps, CV_32S);
    // stable processing frame the pipelines slice into (allocate before slicing)
    if (procFrame.size() != ps || procFrame.type() != CV_8UC3)
	procFrame = cv::Mat::zeros(ps, CV_8UC3);
    pipelineSnapshot = cv::Mat::zeros(ps, CV_8U);
    pipelineSnapshotMarked = cv::Mat::zeros(ps, CV_8U);
    sourceFrame.release();

    if (threadsCount == 0)
	threadsCount = std::thread::hardware_concurrency();

    // re-slice existing pipelines at the new size, preserving their plugins
    if (pipelines.size() > threadsCount)
    {
	int sliceHeight = procHeight / threadsCount;
	int y = 0;
	for (unsigned int i = 0; i < threadsCount; i++)
	{
	    if (i == threadsCount - 1) sliceHeight = procHeight - y;
	    pipelines[i].Reset(cv::Rect(0, y, procWidth, sliceHeight));
	    y += sliceHeight;
	}
	pipelines[threadsCount].Reset(cv::Rect(0, 0, procWidth, procHeight));
    }
}

// change the input downscale factor and rebuild, returning to the same time
void ImageProcessingEngine::SetInputScale(float s)
{
    if (s < 0.1f) s = 0.1f;
    if (s > 1.0f) s = 1.0f;
    if (std::abs(s - inputScale) < 1e-4f) return;

    double keepTime = GetPresentTime();

    inputScale = s;
    CaptureNativeBgZone();
    BuildProcBuffers();

    // rebuild the prefetch buffer at the new resolution and seek back
    frameBuffer.clear();
    playIndex = -1;
    SeekTime(keepTime);
}



void ImageProcessingEngine::LoadXML(cv::FileNode& fn)
{
     // read xml file
     if (!fn.empty())
     {
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
	bgLowThreshold = (int)fn["BackgroundLowThreshold"];
	bgHighThreshold = (int)fn["BackgroundHighThreshold"];

	string bt = (string)fn["BackgroundCalcType"];
	if (bt == "median") bgCalcType = BG_MEDIAN;
	else if (bt == "mean") bgCalcType = BG_MEAN;

	zonesFilename = (string)fn["ZonesFilename"];
    }
}

void ImageProcessingEngine::SaveXML(cv::FileStorage& fs)
{
    fs << "StartTime" << startTime;
    fs << "DurationTime" << durationTime;
    fs << "Timestep" << timestep;
    fs << "UseTimeBounds" << useTimeBoundaries;
    fs << "BackgroundFilename" << bgFilename;
    fs << "BackgroundRecalculate" << bgRecalculate;
    fs << "BackgroundFrames" << bgFrames;
    fs << "BackgroundStartTime" << bgStartTime;
    fs << "BackgroundEndTime" << bgEndTime;
    fs << "BackgroundLowThreshold" << bgLowThreshold;
    fs << "BackgroundHighThreshold" << bgHighThreshold;
    if (bgCalcType == BG_MEDIAN)
	fs << "BackgroundCalcType" << "median";
    else if (bgCalcType == BG_MEAN)
	fs << "BackgroundCalcType" << "mean";
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

    // create new pipelines at the processing resolution
    UpdateProcSize();
    int sliceHeight = procHeight / threadsCount;
    int y = 0;
    for (unsigned int i = 0; i < threadsCount; i++)
    {
	if (i == threadsCount - 1) sliceHeight = procHeight - y;
	pipelines.push_back(Pipeline(this, cv::Rect(0, y, procWidth, sliceHeight)));
	y += sliceHeight;
    }
    // this is the pipeline for the non multithreaded plugins
    pipelines.push_back(Pipeline(this, cv::Rect(0, 0, procWidth, procHeight)));

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

    double ctime = GetPresentTime();

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
    }

    // passed all tests, proceed to image analysis
    nextStepTime += timestep;

    threadsDrawHud = drawHud;
    hud.setTo (0);

    // check if frame is static — based on the process head (the frame the
    // pipeline actually runs on), which advances even when the present frame
    // is held back by the output latency
    lastFrameNumber = frameNumber;
    frameNumber = GetProcessFrameNumber();
    staticFrame = (frameNumber == lastFrameNumber);

    // save a pristine copy of the source frame; on static re-steps restore
    // it so that in-place enhancement plugins are applied only once
    if (!staticFrame
	|| sourceFrame.size() != procFrame.size()
	|| sourceFrame.type() != procFrame.type())
	procFrame.copyTo(sourceFrame);
    else
	sourceFrame.copyTo(procFrame);

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
	cv::threshold(pipelineSnapshotMarked, pipelineSnapshot, 0, 255, cv::THRESH_BINARY);
    }

    // when a centered / delayed plugin is active, keep the processed (enhanced)
    // frame so the delayed display shows the right image, in sync with its mask
    if (outputLatency > 0 && playIndex >= 0 && playIndex < (int)frameBuffer.size())
	procFrame.copyTo(frameBuffer[playIndex].processed);
}

// ---- prefetch buffer -------------------------------------------------------

int ImageProcessingEngine::ComputePrefetch()
{
    int m = 0;
    for (auto& pl : pipelines)
	for (auto* p : pl.plugins)
	    if (p && p->active)
		m = std::max(m, p->PrefetchAhead());
    return m;
}

int ImageProcessingEngine::ComputeOutputLatency()
{
    int m = 0;
    for (auto& pl : pipelines)
	for (auto* p : pl.plugins)
	    if (p && p->active)
		m = std::max(m, p->OutputLatency());
    return m;
}

bool ImageProcessingEngine::DecodeOne()
{
    if (!capture->GetNextFrame()) return false;
    BufferedFrame bf;
    bf.number = capture->GetFrameNumber();
    bf.time = capture->GetTime();
    // store the frame at processing resolution: downscale the decoded frame
    // into the buffer (the decoder's capture->frame stays at source size)
    if (inputScale < 0.999f && procWidth > 0 &&
	(capture->frame.cols != procWidth || capture->frame.rows != procHeight))
	cv::resize(capture->frame, bf.image,
		   cv::Size(procWidth, procHeight), 0, 0, cv::INTER_AREA);
    else
	capture->frame.copyTo(bf.image);
    frameBuffer.push_back(std::move(bf));
    return true;
}

void ImageProcessingEngine::PresentPlayhead()
{
    if (playIndex < 0 || playIndex >= (int)frameBuffer.size()) return;

    // the pipeline runs on the process head; copy it into the stable procFrame
    // in place so the per-thread slice views stay valid
    frameBuffer[playIndex].image.copyTo(procFrame);

    // the present (displayed) frame lags the process head by outputLatency, so
    // a centered temporal-mask result aligns with the image shown
    int pi = playIndex - outputLatency;
    if (pi < 0) pi = 0;
    presentNumber = frameBuffer[pi].number;
    presentTime = frameBuffer[pi].time;
}

cv::Mat ImageProcessingEngine::GetPresentImage()
{
    // no latency: show the (possibly enhanced) process-head frame directly
    if (outputLatency <= 0 || frameBuffer.empty()) return procFrame;
    int pi = playIndex - outputLatency;
    if (pi < 0) pi = 0;
    if (pi >= (int)frameBuffer.size()) pi = (int)frameBuffer.size() - 1;
    // prefer the processed (enhanced) frame so the display matches its mask
    if (!frameBuffer[pi].processed.empty()) return frameBuffer[pi].processed;
    return frameBuffer[pi].image;
}

bool ImageProcessingEngine::AdvanceFrame()
{
    prefetchAhead = ComputePrefetch();
    outputLatency = ComputeOutputLatency();

    if (frameBuffer.empty())
    {
	if (!DecodeOne()) return false;
	playIndex = 0;
    }
    else
    {
	if (playIndex + 1 >= (int)frameBuffer.size())
	    if (!DecodeOne()) return false;
	playIndex++;
    }

    // keep the decoder prefetchAhead frames ahead of the playhead
    while ((int)frameBuffer.size() - 1 - playIndex < prefetchAhead)
	if (!DecodeOne()) break;

    // keep enough past frames for the presentation delay + step-back
    int keepPast = std::max(maxPastFrames, outputLatency + 2);
    while (playIndex > keepPast)
    {
	frameBuffer.pop_front();
	playIndex--;
    }

    PresentPlayhead();

    if (useTimeBoundaries && durationTime > 0.0000001)
	if (presentTime > (startTime + durationTime))
	    return false;

    return true;
}

bool ImageProcessingEngine::StepBackward()
{
    // serve from the cache if the previous frame is buffered
    if (playIndex > 0)
    {
	playIndex--;
	PresentPlayhead();
	return true;
    }

    // not cached: seek (the user may have to wait)
    double fps = capture->GetFPS();
    double target = presentTime - (fps > 0 ? 1.0 / fps : 0.04);
    if (target < 0) target = 0;
    SeekTime(target);
    return true;
}

void ImageProcessingEngine::SeekTime(double t)
{
    capture->GetFrame(t);
    frameBuffer.clear();
    playIndex = -1;
    if (!DecodeOne()) return;
    playIndex = 0;

    prefetchAhead = ComputePrefetch();
    outputLatency = ComputeOutputLatency();
    while ((int)frameBuffer.size() - 1 - playIndex < prefetchAhead)
	if (!DecodeOne()) break;

    PresentPlayhead();
}

cv::Mat ImageProcessingEngine::GetBufferedImage(int offset)
{
    if (frameBuffer.empty()) return cv::Mat();
    int i = playIndex + offset;
    if (i < 0) i = 0;
    if (i >= (int)frameBuffer.size()) i = (int)frameBuffer.size() - 1;
    return frameBuffer[i].image;
}

bool ImageProcessingEngine::GetNextFrame()
{
    // now backed by the prefetch buffer
    return AdvanceFrame();
}
