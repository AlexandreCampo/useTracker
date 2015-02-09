#ifndef CAPTURE_IMAGE_H
#define CAPTURE_IMAGE_H

#include "Capture.h"

#include <string>

struct CaptureImage : public Capture
{
    std::string filename;

    CaptureImage(std::string filename);
    CaptureImage(cv::FileNode& fn);
    ~CaptureImage();

    bool Open(std::string filename);
    void Close();

    void Pause() {};
    void Play() {};
    void Stop() {};

    bool GetNextFrame ();
    wxLongLong GetNextFrameSystemTime();
    bool GetFrame (double time);

    long GetFrameNumber();
    long GetFrameCount();
    double GetTime();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};




#endif //CAPTURE_IMAGE_H
