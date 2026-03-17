#ifndef CAPTURE_MULTI_USB_CAMERA_H
#define CAPTURE_MULTI_USB_CAMERA_H

#include "Capture.h"
#include "CaptureUSBCamera.h"

struct CaptureMultiUSBCamera : public Capture
{
    std::vector<CaptureUSBCamera*> subcaptures;
    std::vector<cv::Rect> rects;

    int masterDevice;
    int deviceToCalibrate;

    std::vector<std::vector<cv::Point2f>> stitchPoints;
    std::vector<cv::Mat> homographies;
    std::vector<cv::Mat> stitchMasks;
    bool stitched;
    bool stitching;

    CaptureMultiUSBCamera(std::vector<int> devices);
    CaptureMultiUSBCamera(cv::FileNode& fn);
    ~CaptureMultiUSBCamera();

    bool Open(std::vector<int> devices);
    bool Open();
    void Close();

    void Pause();
    void Play();
    void Stop();

    bool GetNextFrame ();
    int64_t GetNextFrameSystemTime();
    bool GetFrame (double time);
    double GetTime();
    int64_t InternalGetTime();

    long GetFrameNumber();
    long GetFrameCount();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    std::string GetName();

    void SetDeviceToCalibrate(int d);
    int GetDeviceToCalibrate();

    unsigned int GetDeviceCount();
    std::string GetDeviceName(unsigned int d);

    void CalibrationLoadXML (cv::FileNode& fn);
    void CalibrationSaveXML (cv::FileStorage& fs);

    void CalibrationStart();
    void CalibrationReset();
    void Calibrate();
    cv::Mat CalibrationGetFrame();
    void CalibrationOutputHud(cv::Mat& hud);
    void CalibrationSetBoardType(int type);
    void CalibrationSetBoardWidth(int w);
    void CalibrationSetBoardHeight(int h);
    void CalibrationSetSquareSize(int size);
    void CalibrationSetFramesCount(int framesCount);
    void CalibrationSetAspect(float aspect);
    void CalibrationSetZeroTangentDist(bool zeroTangentDist);
    void CalibrationSetFixPrincipalPoint(bool fixPrincipalPoint);
    void CalibrationSetFlipVertical(bool flipVertical);

    int CalibrationGetBoardType();
    int CalibrationGetBoardWidth();
    int CalibrationGetBoardHeight();
    int CalibrationGetSquareSize();
    int CalibrationGetFramesCount();
    int CalibrationGetAspectNum();
    int CalibrationGetAspectDen();
    float CalibrationGetFrameDelay();
    bool CalibrationGetZeroTangentDist();
    bool CalibrationGetFixPrincipalPoint();
    bool CalibrationGetFlipVertical();

    void ResetStitching();
    bool Stitch();
    bool TryToStitch();
};

#endif //CAPTURE_MULTI_USB_CAMERA_H
