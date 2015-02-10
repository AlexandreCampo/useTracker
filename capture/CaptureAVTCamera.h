#ifdef VIMBA

#ifndef CAPTURE_AVT_CAMERA_H
#define CAPTURE_AVT_CAMERA_H

#include "Capture.h"

#include "vimba/ApiController.h"

class AVT::VmbAPI::ApiController;

struct CaptureAVTCamera : Capture
{
    int device;
    AVT::VmbAPI::ApiController vimbaApiController;

    unsigned int frameNumber = 0;
    wxLongLong startTime = 0;
    wxLongLong pauseTime = 0;
    wxLongLong nextFrameTime = 0;
    wxLongLong playTimestep = 0;

    CaptureAVTCamera(int device);
    CaptureAVTCamera(cv::FileNode& fn);
    ~CaptureAVTCamera();

    bool Open(int device);
    void Close();

    void Pause();
    void Play();
    void Stop();

    bool GetNextFrame ();
    bool GetFrame (double time);
    wxLongLong GetNextFrameSystemTime();
    double GetTime();

    long GetFrameNumber();
    long GetFrameCount();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};




#endif //CAPTURE_AVT_CAMERA_H

#endif // VIMBA
