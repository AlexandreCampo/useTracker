#ifndef CAPTURE_H
#define CAPTURE_H

#include <iostream>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <wx/time.h>

#include "CaptureCalibration.h"

struct Capture
{
    // members
    // ------------------------------------

    enum Type {NONE, IMAGE, VIDEO, USB_CAMERA, AVT_CAMERA, MULTI_USB_CAMERA, MULTI_VIDEO};
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

    virtual std::string GetName() = 0;

    /* virtual unsigned int GetDeviceCount(); */
    /* virtual std::string GetDeviceName(unsigned int d); */

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
