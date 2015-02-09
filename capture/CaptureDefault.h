#ifndef CAPTURE_DEFAULT_H
#define CAPTURE_DEFAULT_H

#include "Capture.h"

#include <string>

struct CaptureDefault : public Capture
{
    CaptureDefault();

    bool GetNextFrame ();
    wxLongLong GetNextFrameSystemTime ();
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
};




#endif //CAPTURE_DEFAULT_H
