#ifdef VIMBA

#include "CaptureAVTCamera.h"

#include <chrono>
#include <thread>
#include <iostream>
//#include <chrono>

#include <unistd.h>

#ifdef VIMBA
#include "vimba/ApiController.h"
#endif //VIMBA

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
	usleep (10);
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
    if (vimbaApiController.FrameAvailable())
	vimbaApiController.GetFrame(frame);
    else return false;

    frameNumber++;
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
    while (GetTime() < time) this_thread::sleep_for(chrono::milliseconds(10)); 

    if (vimbaApiController.FrameAvailable())
	vimbaApiController.GetFrame(frame);
    else return false;

    frameNumber++;
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
    if (isPaused) return (pauseTime - startTime).ToDouble() / 1000000.0;
    else if (isStopped) return 0;
    else return (wxGetUTCTimeUSec() - startTime).ToDouble() / 1000000.0;
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
