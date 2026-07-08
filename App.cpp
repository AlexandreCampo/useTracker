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

#include "App.h"

#include "ImageProcessingEngine.h"

#include "SafeErosion.h"
#include "FrameDifference.h"
#include "Erosion.h"
#include "Dilation.h"
#include "ExtractMotion.h"
#include "BackgroundDiffMOG.h"
#include "BackgroundDiffMOG2.h"
#include "BackgroundDiffGMG.h"
#include "BackgroundDiffGSOC.h"
#include "BackgroundDiffKNN.h"
#include "Clahe.h"
#include "Curves.h"
#include "Denoise.h"
#include "TemporalDenoise.h"
#include "Sharpen.h"
#include "Dehaze.h"
#include "WhiteBalance.h"
#include "YoloDetector.h"
#include "PatternTracker.h"
#include "ColorSegmentation.h"
#include "ExtractBlobs.h"
#include "GetBlobsAngles.h"
#include "Tracker.h"
#include "MovingAverage.h"
#include "RecordVideo.h"
#include "RecordPixels.h"
#include "ZonesOfInterest.h"
#include "SimpleTags.h"
#include "Aruco.h"
#include "Stopwatch.h"
#include "RemoteControl.h"
#include "AdaptiveThreshold.h"
#include "TakeSnapshots.h"
#include "ArucoColor.h"

#include "CaptureVideo.h"
#include "CaptureUSBCamera.h"
#include "CaptureMultiUSBCamera.h"
#include "CaptureMultiVideo.h"
#include "CaptureImage.h"

#include "Parameters.h"
#include "AppGui.h"

#include <opencv2/core.hpp>
#include <filesystem>
#include <iostream>

extern Parameters parameters;
std::map<std::string, std::vector<PipelinePlugin*> (*)(cv::FileNode&, unsigned int)> NewPipelinePluginVector;

int main(int argc, char **argv)
{
    NewPipelinePluginVector["BackgroundDifference"] = &CreatePipelinePluginVector<ExtractMotion>;
    NewPipelinePluginVector["BackgroundDiffMog"] = &CreatePipelinePluginVector<BackgroundDiffMOG>;
    NewPipelinePluginVector["BackgroundDiffMog2"] = &CreatePipelinePluginVector<BackgroundDiffMOG2>;
    NewPipelinePluginVector["BackgroundDiffGmg"] = &CreatePipelinePluginVector<BackgroundDiffGMG>;
    NewPipelinePluginVector["BackgroundDiffGsoc"] = &CreatePipelinePluginVector<BackgroundDiffGSOC>;
    NewPipelinePluginVector["BackgroundDiffKnn"] = &CreatePipelinePluginVector<BackgroundDiffKNN>;
    NewPipelinePluginVector["Clahe"] = &CreatePipelinePluginVector<Clahe>;
    NewPipelinePluginVector["Curves"] = &CreatePipelinePluginVector<Curves>;
    NewPipelinePluginVector["Denoise"] = &CreatePipelinePluginVector<Denoise>;
    NewPipelinePluginVector["TemporalDenoise"] = &CreatePipelinePluginVector<TemporalDenoise>;
    NewPipelinePluginVector["Sharpen"] = &CreatePipelinePluginVector<Sharpen>;
    NewPipelinePluginVector["Dehaze"] = &CreatePipelinePluginVector<Dehaze>;
    NewPipelinePluginVector["WhiteBalance"] = &CreatePipelinePluginVector<WhiteBalance>;
    NewPipelinePluginVector["YoloDetector"] = &CreatePipelinePluginVector<YoloDetector>;
    NewPipelinePluginVector["PatternTracker"] = &CreatePipelinePluginVector<PatternTracker>;
    NewPipelinePluginVector["ColorSegmentation"] = &CreatePipelinePluginVector<ColorSegmentation>;
    NewPipelinePluginVector["Erosion"] = &CreatePipelinePluginVector<Erosion>;
    NewPipelinePluginVector["Dilation"] = &CreatePipelinePluginVector<Dilation>;
    NewPipelinePluginVector["ExtractBlobs"] = &CreatePipelinePluginVector<ExtractBlobs>;
    NewPipelinePluginVector["GetBlobsAngles"] = &CreatePipelinePluginVector<GetBlobsAngles>;
    NewPipelinePluginVector["TrackBlobs"] = &CreatePipelinePluginVector<Tracker>;
    NewPipelinePluginVector["SafeErosion"] = &CreatePipelinePluginVector<SafeErosion>;
    NewPipelinePluginVector["FrameDifference"] = &CreatePipelinePluginVector<FrameDifference>;
    NewPipelinePluginVector["MovingAverage"] = &CreatePipelinePluginVector<MovingAverage>;
    NewPipelinePluginVector["RecordVideo"] = &CreatePipelinePluginVector<RecordVideo>;
    NewPipelinePluginVector["RecordPixels"] = &CreatePipelinePluginVector<RecordPixels>;
    NewPipelinePluginVector["ZonesOfInterest"] = &CreatePipelinePluginVector<ZonesOfInterest>;
    NewPipelinePluginVector["SimpleTags"] = &CreatePipelinePluginVector<SimpleTags>;
    NewPipelinePluginVector["Stopwatch"] = &CreatePipelinePluginVector<Stopwatch>;
    NewPipelinePluginVector["RemoteControl"] = &CreatePipelinePluginVector<RemoteControl>;
    NewPipelinePluginVector["AdaptiveThreshold"] = &CreatePipelinePluginVector<AdaptiveThreshold>;
    NewPipelinePluginVector["TakeSnapshots"] = &CreatePipelinePluginVector<TakeSnapshots>;
    NewPipelinePluginVector["ArucoColor"] = &CreatePipelinePluginVector<ArucoColor>;
    NewPipelinePluginVector["Aruco"] = &CreatePipelinePluginVector<Aruco>;

    // read command line, load parameters
    parameters.parseCommandLine (argc, argv);

    if (parameters.nogui)
    {
        ImageProcessingEngine ipEngine;

        ipEngine.LoadXML (parameters.rootNode);

        // load capture source from command line params if possible
        if (!parameters.inputFilename.empty())
        {
            // check extension
            std::filesystem::path fpath(parameters.inputFilename);
            if (fpath.extension() == ".xml")
            {
                std::string filename = parameters.inputFilename;
                cv::FileStorage file;
                cv::FileNode rootNode;

                file.open(filename, cv::FileStorage::READ);
                if (file.isOpened())
                {
                    rootNode = file["Source"];

                    if (!rootNode.empty())
                    {
                        std::string type = (std::string)rootNode["Type"];

                        if (type == "multiVideo")
                        {
                            ipEngine.capture->LoadXML(rootNode);
                        }
                        else if (type == "video")
                        {
                            ipEngine.capture = new CaptureVideo(rootNode);
                        }
                        else if (type == "USBcamera")
                        {
                            ipEngine.capture = new CaptureUSBCamera(rootNode);
                        }
                        else if (type == "image")
                        {
                            ipEngine.capture = new CaptureImage(rootNode);
                        }
                        else if (type == "multiUSBcamera")
                        {
                            ipEngine.capture = new CaptureMultiUSBCamera(rootNode);
                        }
                    }
                }
            }
            else
            {
                // try to load input as video file
                ipEngine.capture = new CaptureVideo (parameters.inputFilename);

                // if video not loaded, try image
                if (ipEngine.capture->type == Capture::NONE)
                {
                    delete ipEngine.capture;
                    ipEngine.capture = new CaptureImage (parameters.inputFilename);
                }
            }
        }
        else if (parameters.usbDevice >= 0)
        {
            ipEngine.capture = new CaptureUSBCamera (parameters.usbDevice);
        }
        else if (parameters.multiUSBCapture == true)
        {
            CaptureMultiUSBCamera* mu = new CaptureMultiUSBCamera (parameters.usbDevices);
            ipEngine.capture = mu;
            if (mu && !parameters.stitchingFilename.empty())
            {
                cv::FileStorage file (parameters.stitchingFilename, cv::FileStorage::READ);
                if (file.isOpened())
                {
                    cv::FileNode rootNode = file["Source"];
                    mu->LoadXML(rootNode);
                }
            }
        }
        else if (parameters.multiVideoCapture == true)
        {
            CaptureMultiVideo* mv = new CaptureMultiVideo (parameters.inputFilenames);
            ipEngine.capture = mv;
            if (mv && !parameters.stitchingFilename.empty())
            {
                cv::FileStorage file (parameters.stitchingFilename, cv::FileStorage::READ);
                if (file.isOpened())
                {
                    cv::FileNode rootNode = file["Source"];
                    mv->LoadXML(rootNode, true);
                }
            }
        }

        // if capture not loaded, return error
        if (!ipEngine.capture || ipEngine.capture->type == Capture::NONE)
        {
            std::cerr << "Error : Could not open source" << std::endl;
            return 1;
        }

        if (ipEngine.capture && !parameters.calibrationFilename.empty())
        {
            cv::FileStorage file (parameters.calibrationFilename, cv::FileStorage::READ);
            if (file.isOpened())
            {
                cv::FileNode rootNode = file["Calibration"];
                ipEngine.capture->calibration.LoadXML(rootNode);
            }
        }

        if (parameters.inputScale > 0.0f && parameters.inputScale <= 1.0f)
            ipEngine.inputScale = parameters.inputScale;
        ipEngine.Reset(parameters);

        // load pipeline's XML
        cv::FileNode fn = parameters.rootNode["Pipeline"];
        if (!fn.empty())
        {
            cv::FileNodeIterator it = fn.begin(), it_end = fn.end();
            for (; it != it_end; ++it)
            {
                cv::FileNode fn2 = *((*it).begin()); // ugly hack to go around duplicate key bug
                auto pfv = NewPipelinePluginVector[fn2.name()] (fn2, ipEngine.threadsCount);

                ipEngine.PushBack(pfv, true);
            }
        }

        // run the engine
        ipEngine.OpenOutput();
        ipEngine.capture->Play();

        long totalFrames = ipEngine.capture->GetFrameCount();
        if (ipEngine.useTimeBoundaries && ipEngine.durationTime > 0.0000001)
        {
            totalFrames = (long)((ipEngine.startTime + ipEngine.durationTime) * ipEngine.capture->fps);
        }

        long progress = 0;
        long startFrame = (long)(ipEngine.startTime * ipEngine.capture->fps);
        do
        {
            if (ipEngine.timestep < 0.00001 || ipEngine.capture->GetTime() >= ipEngine.nextStepTime)
                ipEngine.Step();

            long frameNumber = ipEngine.capture->GetFrameNumber();
            long newProgress = ((frameNumber - startFrame) * 100) / totalFrames;
            if (newProgress > progress)
            {
                progress = newProgress;
                if (progress > 100) progress = 100;

                std::cout << "Progress : " << progress  << "% | frame " << frameNumber << "/" << totalFrames << " | time " << ipEngine.capture->GetTime() << "s" <<  std::endl;
            }

            // end if outside time boundaries
            if (ipEngine.useTimeBoundaries && ipEngine.durationTime > 0.0000001)
                if (ipEngine.capture->GetTime() > ipEngine.startTime + ipEngine.durationTime)
                    break;
        }
        while (ipEngine.GetNextFrame());

        ipEngine.CloseOutput();

        return 0;
    }
    else
    {
        AppGui gui;
        return gui.Run();
    }
}
