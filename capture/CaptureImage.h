#ifndef CAPTURE_IMAGE_H
#define CAPTURE_IMAGE_H

#include "Capture.h"

#include <string>

struct CaptureImage : public Capture
{
    std::string filename;
//    cv::Mat source;

    CaptureImage(std::string filename);
    CaptureImage(cv::FileNode& fn);
    ~CaptureImage();

    bool Open(std::string filename);
    void Close();

    bool GetNextFrame (bool blocking = false);
    bool GetFrame (double time);

    long GetFrameNumber();
    long GetFrameCount();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};




#endif //CAPTURE_IMAGE_H
