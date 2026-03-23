#ifndef CAPTURE_MULTI_VIDEO_H
#define CAPTURE_MULTI_VIDEO_H

#include "Capture.h"
#include "CaptureVideo.h"

struct CaptureMultiVideo : public Capture
{
    std::vector<CaptureVideo*> subcaptures;

    std::vector<cv::Rect> rects;

    int masterDevice;
    int deviceToCalibrate;

    std::vector<std::vector<cv::Point2f>> stitchPoints;
    std::vector<cv::Mat> homographies;
    std::vector<cv::Mat> stitchMasks;
    bool stitched;
    bool stitching;

    bool adjustBrightness;
    std::vector<cv::Scalar> targetBrightness;

    CaptureMultiVideo(std::vector<std::string> filenames);
    CaptureMultiVideo(cv::FileNode& fn);
    ~CaptureMultiVideo();

    bool Open(std::vector<std::string> filenames);
    bool Open();
    void Close();

    void Pause();
    void Play();
    void Stop();

    void SetSpeedFaster(int speed) override;
    void SetSpeedSlower(int speed) override;
    void SetPlaySpeed(int level) override;

    bool GetNextFrame ();
    bool GetPreviousFrame();
    int64_t GetNextFrameSystemTime();
    bool GetFrame (double time);
    double GetTime();

    void SetTime(double time);

    long GetFrameNumber();
    long GetFrameCount();
    bool SetFrameNumber(long f);

    void LoadXML (cv::FileNode& fn);
    void LoadXML (cv::FileNode& fn, bool stitchingOnly);
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
    void CalibrationSetFrameDelay (float delay);
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

    void CalculateAverageBrightness(Capture* c, cv::Scalar& b);
    void AdjustBrightness (int subcapture);

    void Merge();
};

#endif //CAPTURE_MULTI_VIDEO_H
