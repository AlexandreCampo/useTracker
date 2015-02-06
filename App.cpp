/***************************************************************
 * Name:      App.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2014-08-16
 * Copyright:  ()
 * License:
 **************************************************************/

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

    // read command line, load parameters
    parameters.parseCommandLine (argc, argv);

    if (parameters.nogui)
    {
        ImageProcessingEngine ipEngine;

	ipEngine.LoadXML (parameters.rootNode);
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

	while (ipEngine.GetNextFrame())
	{
	    ipEngine.Step();
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
    wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "Tracker");
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
    	mainFrame = new MainFrame(0);
    	mainFrame->Show();
    	SetTopWindow(mainFrame);
    }
    //*)
    return wxsOK;
}


int App::FilterEvent(wxEvent& event)
{
    if (mainFrame) return mainFrame->FilterEvent(event);
    return -1;
}
