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
    Open(device);
    type = AVT_CAMERA;
}

CaptureAVTCamera::CaptureAVTCamera(FileNode& fn) : Capture()
{
    LoadXML(fn);
    Open(device);
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
	    err = cameras[0]->GetID(cameraID);
	}
    }
    if (err == VmbErrorSuccess)
    {
	std::cout<<"Opening camera with ID: "<< cameraID <<"\n";

	err = vimbaApiController.StartContinuousImageAcquisition(cameraID);

	if (err == VmbErrorSuccess)
	{
	    // std::cout<< "Press <enter> to stop aquision...\n" ;
	    // getchar();

	    // vimbaApiController.StopContinuousImageAcquisition();
	}
    }

    if (err != VmbErrorSuccess)
    {
    	std::string strError = vimbaApiController.ErrorCodeToMessage( err );
    	std::cout<<"\nAn error occured:"<<strError<<"\n";

	exit(-1);
    }

    // Get the properties from the camera
    width = vimbaApiController.GetWidth();
    height = vimbaApiController.GetHeight();

    // estimate FPS by retrieving xx frames and measuring time
    double t1 = GetTime ();
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
    double t2 = GetTime ();
    fps = 20.0 / (t2 - t1);

    std::cout << "Camera properties\n";
    std::cout << "width = " << width << std::endl <<" height = "<< height << " fps = " << fps << std::endl;

//    currentTime = 0.0;

    // cameraSource >> frame;
    frame = Mat (height, width, CV_8UC3);

    type = AVT_CAMERA;
}

void CaptureAVTCamera::Close ()
{

}

bool CaptureAVTCamera::GetNextFrame (bool blocking)
{
    if (vimbaApiController.FrameAvailable())
	vimbaApiController.GetFrame(frame);
    else return false;

    GetTime();
    frameNumber++;

    return !frame.empty();
}

bool CaptureAVTCamera::GetFrame (double time)
{
    while (GetTime() < time) this_thread::sleep_for(chrono::milliseconds(10)); 

    if (vimbaApiController.FrameAvailable())
	vimbaApiController.GetFrame(frame);
    else return false;

    frameNumber++;

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


// double CaptureAVTCamera::GetVideoTime()
// {
//     return (double)source.get(CV_CAP_PROP_POS_MSEC) / 1000.0;
// }

// float TrackerMain::GetVideoTimeRatio()
// {
//     return source.get(CV_CAP_PROP_POS_AVI_RATIO);
// }


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
