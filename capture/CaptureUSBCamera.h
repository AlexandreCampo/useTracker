#ifndef CAPTURE_USB_CAMERA_H
#define CAPTURE_USB_CAMERA_H

#include "Capture.h"

struct CaptureUSBCamera : public Capture
{
    int device;
    cv::VideoCapture source;

    CaptureUSBCamera(int device);
    CaptureUSBCamera(cv::FileNode& fn);
    ~CaptureUSBCamera();

    bool Open(int device);
    void Close();

    bool GetNextFrame (bool blocking = false);
    bool GetFrame (double time);
//    double GetTime();

    //bool EndReached();
    long GetFrameNumber();
    long GetFrameCount();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};




#endif //CAPTURE_USB_CAMERA_H
