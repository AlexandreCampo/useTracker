#ifndef CAPTURE_DEFAULT_H
#define CAPTURE_DEFAULT_H

#include "Capture.h"

#include <string>

struct CaptureDefault : public Capture
{
    CaptureDefault();

    bool GetNextFrame (bool blocking = false);
    bool GetFrame (double time);

    long GetFrameNumber();
    long GetFrameCount();

    void Close() {};
    void LoadXML (cv::FileNode& fn) {};
    void SaveXML (cv::FileStorage& fs) {};
};




#endif //CAPTURE_DEFAULT_H
