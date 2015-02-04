#ifndef CAPTURE_H
#define CAPTURE_H

#include <string>
#include <opencv2/highgui/highgui.hpp>

struct Capture
{
    // members
    // ------------------------------------

    enum Type {IMAGE, VIDEO, USB_CAMERA, AVT_CAMERA};
    Type type = IMAGE;

    // time management
    double systime;
    double initTime;
    double time;
    double lastTime;
    float timestep;

    float startTime;
    float duration;

#ifdef WIN32
    double frequency;
#endif

    // properties
    int width;
    int height;
    double fps;

    // output
    cv::Mat frame;

    unsigned int frameNumber = 0;

    // methods
    // -----------------------------
    Capture ();
    virtual ~Capture() {};

    virtual bool GetNextFrame (bool needed) = 0;
    virtual bool GetFrame (double time) = 0;

    /* virtual bool Open (std::string filename) {}; */
    /* virtual bool Open (int device) {}; */
    virtual void Close () = 0;

    double GetSystemTime();
    virtual double GetTime();
    virtual long GetFrameNumber() = 0;
    virtual long GetFrameCount() = 0;
    virtual void SetFrameNumber(long f) {};

    virtual void Stop() {};

    void SetStartTime (float time);
    void SetDuration(float time);
    void SetTimestep(float t);

    virtual void LoadXML (cv::FileNode& fn) = 0;
    virtual void SaveXML (cv::FileStorage& fs) = 0;
};


#endif // CAPTURE_H
