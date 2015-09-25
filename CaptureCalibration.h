
#ifndef CAPTURE_CALIBRATION_H
#define CAPTURE_CALIBRATION_H

#include <string>
#include <opencv2/highgui/highgui.hpp>

class Capture;

class CaptureCalibration
{
public:

    CaptureCalibration();

    enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

    void SaveXML(cv::FileStorage& fs);
    void LoadXML(cv::FileNode& fn);
    void CalibrationLoadXML(cv::FileNode& fn); // performs extra checks (for UI)
    void Start ();
    void Calibrate ();
    void Reset ();
    bool CalculateCalibration();
    void CalcBoardCornerPositions(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners, Pattern patternType /*= Settings::CHESSBOARD*/);
    double ComputeReprojectionErrors( const std::vector<std::vector<cv::Point3f> >& objectPoints, const std::vector<std::vector<cv::Point2f> >& imagePoints);

    void OutputHud (cv::Mat& hud);
    void Undistort (cv::Mat& frame);
    void UpdateFlags();

public:

    Capture* capture;
    cv::Size boardSize;            // The size of the board -> Number of items by width and height
    Pattern pattern;// One of the Chessboard, circles, or asymmetric circle pattern
    float squareSize;          // The size of a square in your defined unit (point, millimeter,etc).
    int nrFrames;              // The number of frames to use from the input for calibration
    float aspectRatio;         // The aspect ratio
    bool firstAnalysisStep;
    double delay;                 // In case of a video input
    double lastAnalysisTime;
    bool zeroTangentDist; // Assume zero tangential distortion
    bool fixPrincipalPoint;// Fix the principal point at the center
    bool flipVertical;          // Flip the captured images around the horizontal axis
    int flags;
    std::vector<cv::Point2f> pointBuf;
    bool boardFound;
    int calibratedImageWidth;
    int calibratedImageHeight;

    std::string patternString;

    std::vector<std::vector<cv::Point2f> > imagePoints;
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;

    bool calibrated;
    bool calibrating;
    int analyzedFrames;
    cv::Mat undistortedMap1, undistortedMap2;

    std::vector<cv::Mat> rvecs, tvecs;
    std::vector<float> reprojErrs;
    double totalAvgErr = 0;

    // about frames
    int frameNumber = 0;
    int lastFrameNumber = 0;
};


#endif
