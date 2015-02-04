#ifndef CAPTURE_VIDEO_H
#define CAPTURE_VIDEO_H

#include "Capture.h"

struct CaptureVideo : Capture
{
    std::string filename;
    cv::VideoCapture source;

    CaptureVideo(std::string filename);
    CaptureVideo(cv::FileNode& fn);
    ~CaptureVideo();

    bool Open(std::string filename);
    void Close();

    bool GetNextFrame (bool needed = false);
    bool GetPreviousFrame(bool needed = true);
    bool GetFrame (double time);

    void Stop();

    long GetFrameNumber();
    long GetFrameCount();
    void SetFrameNumber(long f);

    void SetTime(double time);
    double GetTime();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};




#endif //CAPTURE_VIDEO_H
