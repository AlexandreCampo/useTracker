#ifndef CAPTURE_H
#define CAPTURE_H

#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <wx/time.h>

struct Capture
{
    // members
    // ------------------------------------

    enum Type {NONE, IMAGE, VIDEO, USB_CAMERA, AVT_CAMERA};
    Type type = NONE;

    // properties
    int width;
    int height;
    double fps;

    // output
    cv::Mat frame;
    
    bool isPaused = false;
    bool isStopped = true;
    bool statusChanged = true;

    // methods
    // -----------------------------
    virtual ~Capture() {};

    virtual bool GetNextFrame () = 0;
    virtual wxLongLong GetNextFrameSystemTime () = 0;
    virtual bool GetFrame (double time) = 0;

    virtual void Close () = 0;

    virtual double GetTime() = 0;
    virtual long GetFrameNumber() = 0;
    virtual long GetFrameCount() = 0;

    virtual void Play() = 0;
    virtual void Stop() = 0;
    virtual void Pause() = 0;

    int GetWidth() {return width;};
    int GetHeight() {return height;};
    double GetFPS() {return fps;};

    virtual void LoadXML (cv::FileNode& fn) = 0;
    virtual void SaveXML (cv::FileStorage& fs) = 0;
};


#endif // CAPTURE_H
