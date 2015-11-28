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

//#include <tinyxml2.h>

#include "ImageProcessingEngine.h"

#include "SafeErosion.h"
#include "FrameDifference.h"
#include "Erosion.h"
#include "Dilation.h"
#include "ExtractMotion.h"
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

#include "CaptureVideo.h"
#include "CaptureUSBCamera.h"
#include "CaptureMultiUSBCamera.h"
#include "CaptureMultiVideo.h"
#include "CaptureImage.h"
#ifdef VIMBA
#include "CaptureAVTCamera.h"
#endif

#include "Parameters.h"

#include <opencv2/core/core.hpp>

//(*AppHeaders
#include "MainFrame.h"
#include <wx/xrc/xmlres.h>
#include <wx/image.h>
//*)


wxIMPLEMENT_WX_THEME_SUPPORT

extern Parameters parameters;
std::map<std::string, vector<PipelinePlugin*> (*)(cv::FileNode&, unsigned int)> NewPipelinePluginVector;

int main(int argc, char **argv)
{
    NewPipelinePluginVector["BackgroundDifference"] = &CreatePipelinePluginVector<ExtractMotion>;
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

    #ifdef ARUCO
    NewPipelinePluginVector["Aruco"] = &CreatePipelinePluginVector<Aruco>;
    #endif

    // read command line, load parameters
    parameters.parseCommandLine (argc, argv);

    if (parameters.nogui)
    {
        ImageProcessingEngine ipEngine;
	
	ipEngine.LoadXML (parameters.rootNode);

	// TODO this whole part duplicated from MainFrame.cpp
	// load capture source from command line params if possible
	if (!parameters.inputFilename.empty())
	{
	    // check extension
	    wxFileName f (parameters.inputFilename);
	    if (f.GetExt() == "xml")
	    {
		// TODO duplicated from dialog open capture. Needs a cleaner solution
		std::string filename = parameters.inputFilename;	    
		cv::FileStorage file;
		cv::FileNode rootNode;
		
		file.open(filename, cv::FileStorage::READ);
		if (file.isOpened())
		{
		    rootNode = file["Source"];
		    
		    if (!rootNode.empty())
		    {
			string type = (string)rootNode["Type"];
			
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
#ifdef VIMBA
			else if (type == "AVTcamera")
			{
			    ipEngine.capture = new CaptureAVTCamera(rootNode);
			}
#endif // VIMBA
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
	else if (parameters.avtDevice >= 0)
	{
#ifdef VIMBA
	    ipEngine.capture = new CaptureAVTCamera (parameters.avtDevice);
#endif //VIMBA
	}
	else if (parameters.multiUSBCapture == true)
	{
	    CaptureMultiUSBCamera* mu = new CaptureMultiUSBCamera (parameters.usbDevices);
	    ipEngine.capture = mu;
	    if (mu && !parameters.stitchingFilename.empty())
	    {
		cv::FileStorage file (parameters.stitchingFilename, FileStorage::READ);
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
		cv::FileStorage file (parameters.stitchingFilename, FileStorage::READ);
		if (file.isOpened())
		{
		    cv::FileNode rootNode = file["Source"];
		    mv->LoadXML(rootNode, true);
		}
	    }
	}
	
	if (ipEngine.capture && !parameters.calibrationFilename.empty())
	{
	    cv::FileStorage file (parameters.calibrationFilename, FileStorage::READ);
	    if (file.isOpened())
	    {
		cv::FileNode rootNode = file["Calibration"];
		ipEngine.capture->calibration.LoadXML(rootNode);
	    }
	}
	
	

// 	// load capture source from command line params if possible
// 	if (!parameters.inputFilename.empty())
// 	{
// 	    // try to load input as video
// 	    ipEngine.capture = new CaptureVideo (parameters.inputFilename);
	    
// 	    // if video not loaded, try image
// 	    if (ipEngine.capture->type == Capture::NONE)
// 	    {
// 		delete ipEngine.capture;
// 		ipEngine.capture = new CaptureImage (parameters.inputFilename);
// 	    }
// 	}
// 	else if (parameters.usbDevice >= 0)
// 	{
// 	    ipEngine.capture = new CaptureUSBCamera (parameters.usbDevice);
// 	}
// 	else if (parameters.avtDevice >= 0)
// 	{
// #ifdef VIMBA
// 	    ipEngine.capture = new CaptureAVTCamera (parameters.avtDevice);
// #endif //VIMBA
// 	}
	
	ipEngine.Reset(parameters);
	
	// load pipeline's XML
	FileNode fn = parameters.rootNode["Pipeline"];
	if (!fn.empty())
	{
	    FileNodeIterator it = fn.begin(), it_end = fn.end();
	    for (; it != it_end; ++it)
	    {
		FileNode fn2 = *((*it).begin()); // ugly hack to go around duplicate key bug
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
	    // this maybe an approximation if fps is not accurate
	    totalFrames = (ipEngine.startTime + ipEngine.durationTime) * ipEngine.capture->fps;
	}
	
	long progress = 0;
	while (ipEngine.GetNextFrame())
	{
	    ipEngine.Step();
	    long frameNumber = ipEngine.capture->GetFrameNumber();
	    long newProgress = (frameNumber * 100) / totalFrames;
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

	ipEngine.CloseOutput();

	// end
	return false;
    }
    else
    {
	wxDISABLE_DEBUG_SUPPORT();
	return wxEntry(argc, argv);
    }
}

App& wxGetApp()
{
    return *static_cast<App*>(wxApp::GetInstance());
}

wxAppConsole *wxCreateApp()
{
    wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "useTracker");
    return new App;
}

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);



bool App::OnInit()
{

    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    wxXmlResource::Get()->InitAllHandlers();
    if ( wxsOK )
    {
    	MainFrame* Frame = new MainFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;
}
