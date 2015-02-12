#ifndef CAPTURE_VIDEO_H
#define CAPTURE_VIDEO_H

#include "Capture.h"

struct CaptureVideo : Capture
{
    std::string filename;
    cv::VideoCapture source;


    unsigned int frameNumber = 0;
    wxLongLong startTime = 0;
    wxLongLong nextFrameTime = 0;
    wxLongLong playTimestep = 0;
    wxLongLong time = 0;
    int playSpeed = 0;


    CaptureVideo(std::string filename);
    CaptureVideo(cv::FileNode& fn);
    ~CaptureVideo();

    bool Open(std::string filename);
    void Close();

    bool GetNextFrame ();
    bool GetPreviousFrame();
    wxLongLong GetNextFrameSystemTime();

    bool GetFrame (double time);

    void Start();
    void Stop();

    void Play();
    void Pause();

    void SetSpeedFaster(int speed);
    void SetSpeedSlower(int speed);

    long GetFrameNumber();
    long GetFrameCount();
    bool SetFrameNumber(long f);

    void SetTime(double time);
    double GetTime();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};




#endif //CAPTURE_VIDEO_H
