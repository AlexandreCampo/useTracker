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

#include "Capture.h"

Capture::Capture()
{
    // std::cout << "Setting correct parent ptr for calib" << std::endl;
    calibration.capture = this;
}

void Capture::CalibrationLoadXML (cv::FileNode& fn)
{
    // method with extra checks
    calibration.CalibrationLoadXML (fn);
}

void Capture::CalibrationSaveXML (cv::FileStorage& fs)
{
    calibration.SaveXML (fs);
}

void Capture::CalibrationStart ()
{
    calibration.Start();
}

void Capture::CalibrationReset ()
{
    calibration.Reset();
}

cv::Mat Capture::CalibrationGetFrame()
{
    return frame;
}

void Capture::CalibrationOutputHud(cv::Mat& hud)
{
    calibration.OutputHud(hud);
}

void Capture::Calibrate()
{
    calibration.Calibrate();
}

void Capture::CalibrationSetBoardType(int type)
{
    if (type == 0)
    {
	calibration.pattern = CaptureCalibration::CHESSBOARD;
	calibration.patternString = "CHESSBOARD";
    }
    else if (type == 1)
    {
	calibration.pattern = CaptureCalibration::CIRCLES_GRID;
	calibration.patternString = "CIRCLES_GRID";
    }
    else if (type == 2)
    {
	calibration.pattern = CaptureCalibration::ASYMMETRIC_CIRCLES_GRID;
	calibration.patternString = "ASYMMETRIC_CIRCLES_GRID";
    }
}

void Capture::CalibrationSetBoardWidth(int w)
{
    calibration.boardSize.width = w;
}

void Capture::CalibrationSetBoardHeight(int h)
{
    calibration.boardSize.height = h;
}

void Capture::CalibrationSetSquareSize(int size)
{
    calibration.squareSize = ((float)size / 1000.0);
}

void Capture::CalibrationSetFramesCount(int framesCount)
{
    calibration.nrFrames = framesCount;
}

void Capture::CalibrationSetAspect(float aspect)
{
    calibration.aspectRatio = aspect;
}

void Capture::CalibrationSetZeroTangentDist(bool zeroTangentDist)
{
    calibration.zeroTangentDist = zeroTangentDist;
    calibration.UpdateFlags();
}

void Capture::CalibrationSetFixPrincipalPoint(bool fixPrincipalPoint)
{
    calibration.fixPrincipalPoint = fixPrincipalPoint;
    calibration.UpdateFlags();
}

void Capture::CalibrationSetFlipVertical(bool flipVertical)
{
    calibration.flipVertical = flipVertical;
    calibration.UpdateFlags();
}

void Capture::CalibrationSetFrameDelay (float delay)
{
    calibration.delay = delay;
}

int Capture::CalibrationGetBoardType()
{
    switch (calibration.pattern)
    {
    case CaptureCalibration::CHESSBOARD :
	return 0;
    case CaptureCalibration::CIRCLES_GRID :
	return 1;
    case CaptureCalibration::ASYMMETRIC_CIRCLES_GRID :
	return 2;
    }
}

int Capture::CalibrationGetBoardWidth()
{
    return calibration.boardSize.width;
}

int Capture::CalibrationGetBoardHeight()
{
    return calibration.boardSize.height;
}

int Capture::CalibrationGetSquareSize()
{
    return int(calibration.squareSize * 1000.0);
}

int Capture::CalibrationGetFramesCount()
{
    return calibration.nrFrames;
}

int Capture::CalibrationGetAspectNum()
{
    return 0;
}

int Capture::CalibrationGetAspectDen()
{
    return 0;
}

float Capture::CalibrationGetFrameDelay()
{
    return calibration.delay;
}

bool Capture::CalibrationGetZeroTangentDist()
{
    return calibration.zeroTangentDist;
}

bool Capture::CalibrationGetFixPrincipalPoint()
{
    return calibration.fixPrincipalPoint;
}

bool Capture::CalibrationGetFlipVertical()
{
    return calibration.flipVertical;
}

