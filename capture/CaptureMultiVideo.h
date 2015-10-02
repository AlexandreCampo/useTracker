/*----------------------------------------------------------------------------*/
/*    Copyright (C) 2015 Alexandre Campo                                      */
/*                                                                            */
/*    This file is part of USE Tracker.                                       */
/*                                                                            */
/*    USE Tracker is free software: you can redistribute it and/or modify     */
/*    it under the terms of the GNU General Public License as published by    */
/*    the Free Software Foundation, either version 3 of the License, or       */
/*    (at your option) any later version.                                     */
/*                                                                            */
/*    USE Tracker is distributed in the hope that it will be useful,          */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/*    GNU General Public License for more details.                            */
/*                                                                            */
/*    You should have received a copy of the GNU General Public License       */
/*    along with USE Tracker.  If not, see <http://www.gnu.org/licenses/>.    */
/*----------------------------------------------------------------------------*/

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



    // methods
    CaptureMultiVideo(std::vector<std::string> filenames);
    CaptureMultiVideo(cv::FileNode& fn);
    ~CaptureMultiVideo();

    bool Open(std::vector<std::string> filenames);
    bool Open();
    void Close();

    void Pause();
    void Play();
    void Stop();

    void SetSpeedFaster(int speed);
    void SetSpeedSlower(int speed);

    bool GetNextFrame ();
    bool GetPreviousFrame();
    wxLongLong GetNextFrameSystemTime();
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

    // calibration methods
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
};


#endif //CAPTURE_MULTI_VIDEO_H
