#ifdef VIMBA

#include "CaptureAVTCamera.h"

#include <chrono>
#include <thread>
#include <iostream>
//#include <chrono>

#include <wx/utils.h>
#include "vimba/ApiController.h"

using namespace std;
using namespace cv;


CaptureAVTCamera::CaptureAVTCamera(int device) : Capture()
{
    if (Open(device))
	type = AVT_CAMERA;
}

CaptureAVTCamera::CaptureAVTCamera(FileNode& fn) : Capture()
{
    LoadXML(fn);
    if (Open(device))
	type = AVT_CAMERA;
}

CaptureAVTCamera::~CaptureAVTCamera()
{

}

string CaptureAVTCamera::GetName()
{
    string str = "AVT Camera device ";
    return str + std::to_string(device);
}

bool CaptureAVTCamera::Open (int device)
{
    this->device = device;
    std::string cameraID;
    VmbErrorType err = VmbErrorSuccess;

    err = vimbaApiController.StartUp();
    if (err == VmbErrorSuccess)
    {
	AVT::VmbAPI::CameraPtrVector cameras = vimbaApiController.GetCameraList();
	if (cameras.empty())
	{
	    err = VmbErrorNotFound;
	}
	else
	{
	    err = cameras[device]->GetID(cameraID);
	}
    }
    if (err == VmbErrorSuccess)
    {
	std::cout<<"Opening camera with ID: "<< cameraID <<"\n";
	err = vimbaApiController.StartContinuousImageAcquisition(cameraID);
    }

    if (err != VmbErrorSuccess)
    {
    	std::string strError = vimbaApiController.ErrorCodeToMessage( err );
    	std::cout<<"\nAn error occured:"<<strError<<"\n";
	return false;
    }

    // Get the properties from the camera
    width = vimbaApiController.GetWidth();
    height = vimbaApiController.GetHeight();

    // estimate FPS by retrieving xx frames and measuring time
    wxLongLong t1 = wxGetUTCTimeUSec();
    int frameCount = 0;
    while (1)
    {
	if (vimbaApiController.FrameAvailable())
	{
	    unsigned char* buffer;
	    AVT::VmbAPI::FramePtr pFrame = vimbaApiController.GetFrame();
	    VmbErrorType err = SP_ACCESS( pFrame )->GetImage( buffer );
            if( err == VmbErrorSuccess ) frameCount++;
	    vimbaApiController.QueueFrame(pFrame);

	    if (frameCount == 20) break;
	}
	wxMicroSleep (10);
    }
    wxLongLong t2 = wxGetUTCTimeUSec();
    double delay = (t2 - t1).ToDouble() / 1000000.0;
    fps = 20.0 / delay;

    std::cout << "Camera properties\n";
    std::cout << "width = " << width << std::endl <<" height = "<< height << " fps = " << fps << std::endl;

    playTimestep.Assign(1000000.0 / fps);
    startTime = wxGetUTCTimeUSec();
    pauseTime = startTime;
    isPaused = true;

    frame = Mat (height, width, CV_8UC3);

    return (!frame.empty());
}

void CaptureAVTCamera::Close ()
{

}

bool CaptureAVTCamera::GetNextFrame ()
{
    bool gotFrame = false;
    wxLongLong startTime = wxGetUTCTimeUSec();

    // try to get a frame during x useconds
    while (1)
    {
	if (vimbaApiController.FrameAvailable())
	{
	    gotFrame = vimbaApiController.GetFrame(frame);
	}

	if (gotFrame) break;
	else if (wxGetUTCTimeUSec() - startTime > 100000)
	{
	    return false;
	}

	// did not get a frame, wait a bit before retrying
	wxMicroSleep (100);
//	std::cout << "Waiting for a frame... " << startTime << "\t" << wxGetUTCTimeUSec() << std::endl;
    }

    frameNumber++;
    lastFrameTime = InternalGetTime();
    nextFrameTime += playTimestep;

    return !frame.empty();
}

wxLongLong CaptureAVTCamera::GetNextFrameSystemTime()
{
    return nextFrameTime;
}

void CaptureAVTCamera::Stop()
{
    isPaused = false;
    isStopped = true;
    statusChanged = true;
    frameNumber = 0;
}

void CaptureAVTCamera::Pause()
{
    isPaused = true;
    statusChanged = true;
    pauseTime = wxGetUTCTimeUSec();
}

void CaptureAVTCamera::Play()
{
    if (isPaused)
    {
	startTime += wxGetUTCTimeUSec() - pauseTime;
	nextFrameTime = wxGetUTCTimeUSec() + playTimestep;
	statusChanged = true;
	isPaused = false;
    }
    if (isStopped)
    {
	startTime = wxGetUTCTimeUSec();
	nextFrameTime = startTime + playTimestep;
	statusChanged = true;
	isStopped =false;
    }
}

bool CaptureAVTCamera::GetFrame (double time)
{
    while (InternalGetTime() < time) this_thread::sleep_for(chrono::milliseconds(10));

    if (vimbaApiController.FrameAvailable())
	vimbaApiController.GetFrame(frame);
    else return false;

    frameNumber++;
    lastFrameTime = InternalGetTime();
    nextFrameTime = wxGetUTCTimeUSec() + playTimestep;

    return !frame.empty();
}

long CaptureAVTCamera::GetFrameNumber ()
{
    return frameNumber; // source.get(CV_CAP_PROP_POS_FRAMES);
}

long CaptureAVTCamera::GetFrameCount ()
{
    return 1;
}

double CaptureAVTCamera::GetTime()
{
    if (isStopped) return 0;
    else return lastFrameTime.ToDouble() / 1000000.0;
}

wxLongLong CaptureAVTCamera::InternalGetTime()
{
    if (isPaused) return (pauseTime - startTime);
    else if (isStopped) return 0;
    else return (wxGetUTCTimeUSec() - startTime);
}


void CaptureAVTCamera::SaveXML(FileStorage& fs)
{
    fs << "Source" << "{" << "Type";
    fs << "AVTcamera" << "Device" << device;
    fs << "}";
}

void CaptureAVTCamera::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
	device = (int)fn["Device"];
    }
}

#endif // VIMBA
