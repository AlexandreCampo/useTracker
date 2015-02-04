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


    CaptureAVTCamera(int device);
    CaptureAVTCamera(cv::FileNode& fn);
    ~CaptureAVTCamera();

    bool Open(int device);
    void Close();

    bool GetNextFrame (bool blocking = false);
    bool GetFrame (double time);
//    double GetVideoTime

    long GetFrameNumber();
    long GetFrameCount();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};




#endif //CAPTURE_AVT_CAMERA_H

#endif // VIMBA
