#ifndef CAPTURE_USB_CAMERA_H
#define CAPTURE_USB_CAMERA_H

#include "Capture.h"

struct CaptureUSBCamera : public Capture
{
    int device;
    cv::VideoCapture source;

    unsigned int frameNumber = 0;
    wxLongLong startTime = 0;
    wxLongLong pauseTime = 0;
    wxLongLong nextFrameTime = 0;
    wxLongLong playTimestep = 0;

    CaptureUSBCamera(int device);
    CaptureUSBCamera(cv::FileNode& fn);
    ~CaptureUSBCamera();

    bool Open(int device);
    void Close();

    void Pause();
    void Play();
    void Stop();

    bool GetNextFrame ();
    wxLongLong GetNextFrameSystemTime();
    bool GetFrame (double time);
    double GetTime();

    long GetFrameNumber();
    long GetFrameCount();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};




#endif //CAPTURE_USB_CAMERA_H
