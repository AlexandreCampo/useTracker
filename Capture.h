#ifndef CAPTURE_H
#define CAPTURE_H

#include <iostream>
#include <string>
#include <chrono>
#include <cstdint>
#include <opencv2/highgui.hpp>

#include "CaptureCalibration.h"

// Replacement for wxGetUTCTimeUSec()
inline int64_t GetUTCTimeUSec()
{
    using namespace std::chrono;
    return duration_cast<microseconds>(
        steady_clock::now().time_since_epoch()).count();
}

struct Capture
{
    // members
    // ------------------------------------

    enum Type {NONE, IMAGE, VIDEO, USB_CAMERA, MULTI_USB_CAMERA, MULTI_VIDEO};
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

    std::string name;

    CaptureCalibration calibration;

    // methods
    // -----------------------------
    Capture();
    virtual ~Capture() {};

    virtual bool GetNextFrame () = 0;
    virtual int64_t GetNextFrameSystemTime () = 0;
    virtual bool GetFrame (double time) = 0;

    virtual void Close () = 0;

    virtual double GetTime() = 0;
    virtual long GetFrameNumber() = 0;
    virtual long GetFrameCount() = 0;

    virtual void Play() = 0;
    virtual void Stop() = 0;
    virtual void Pause() = 0;

    virtual void SetSpeedFaster(int speed) {};
    virtual void SetSpeedSlower(int speed) {};
    virtual void SetPlaySpeed(int level) {};

    int GetWidth() {return width;};
    int GetHeight() {return height;};
    double GetFPS() {return fps;};

    virtual void LoadXML (cv::FileNode& fn) = 0;
    virtual void SaveXML (cv::FileStorage& fs) = 0;

    virtual std::string GetName() = 0;

    // calibration methods
    virtual void CalibrationLoadXML (cv::FileNode& fn);
    virtual void CalibrationSaveXML (cv::FileStorage& fs);

    virtual void CalibrationStart();
    virtual void CalibrationReset();
    virtual void Calibrate();
    virtual cv::Mat CalibrationGetFrame();
    virtual void CalibrationOutputHud(cv::Mat& hud);
    virtual void CalibrationSetBoardType(int type);
    virtual void CalibrationSetBoardWidth(int w);
    virtual void CalibrationSetBoardHeight(int h);
    virtual void CalibrationSetSquareSize(int size);
    virtual void CalibrationSetFramesCount(int framesCount);
    virtual void CalibrationSetAspect(float aspect);
    virtual void CalibrationSetZeroTangentDist(bool zeroTangentDist);
    virtual void CalibrationSetFixPrincipalPoint(bool fixPrincipalPoint);
    virtual void CalibrationSetFlipVertical(bool flipVertical);
    virtual void CalibrationSetFrameDelay(float delay);

    virtual int CalibrationGetBoardType();
    virtual int CalibrationGetBoardWidth();
    virtual int CalibrationGetBoardHeight();
    virtual int CalibrationGetSquareSize();
    virtual int CalibrationGetFramesCount();
    virtual int CalibrationGetAspectNum();
    virtual int CalibrationGetAspectDen();
    virtual float CalibrationGetFrameDelay();
    virtual bool CalibrationGetZeroTangentDist();
    virtual bool CalibrationGetFixPrincipalPoint();
    virtual bool CalibrationGetFlipVertical();

};


#endif // CAPTURE_H
