#ifndef CAPTURE_USB_CAMERA_H
#define CAPTURE_USB_CAMERA_H

#include "Capture.h"

struct CaptureUSBCamera : public Capture
{
    int device;
    cv::VideoCapture source;

    unsigned int frameNumber = 0;
    int64_t startTime = 0;
    int64_t pauseTime = 0;
    int64_t nextFrameTime = 0;
    int64_t lastFrameTime = 0;
    int64_t playTimestep = 0;

    CaptureUSBCamera(int device);
    CaptureUSBCamera(cv::FileNode& fn);
    ~CaptureUSBCamera();

    bool Open(int device);
    void Close();

    void Pause();
    void Play();
    void Stop();

    bool GetNextFrame ();
    int64_t GetNextFrameSystemTime();
    bool GetFrame (double time);
    double GetTime();
    int64_t InternalGetTime();

    long GetFrameNumber();
    long GetFrameCount();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    std::string GetName();
};

#endif //CAPTURE_USB_CAMERA_H
