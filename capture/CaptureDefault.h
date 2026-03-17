#ifndef CAPTURE_DEFAULT_H
#define CAPTURE_DEFAULT_H

#include "Capture.h"
#include <string>

struct CaptureDefault : public Capture
{
    CaptureDefault();

    bool GetNextFrame ();
    int64_t GetNextFrameSystemTime ();
    bool GetFrame (double time);

    void Pause() {};
    void Play() {};
    void Stop() {};

    long GetFrameNumber();
    long GetFrameCount();
    double GetTime();

    void Close() {};
    void LoadXML (cv::FileNode& fn) {};
    void SaveXML (cv::FileStorage& fs) {};

    std::string GetName() {return std::string();};
};

#endif //CAPTURE_DEFAULT_H
