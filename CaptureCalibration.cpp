
#include "CaptureCalibration.h"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "Capture.h"

// TODO DEBUG
#include "Utils.h"

using namespace std;
using namespace cv;

CaptureCalibration::CaptureCalibration()
{
    calibrated = false;
    calibrating = false;
    lastAnalysisTime = 0;
    firstAnalysisStep = true;
    boardFound = false;

    boardSize.width = 9;
    boardSize.height = 6;
    squareSize = 50;

    pattern = CHESSBOARD;
    patternString = "CHESSBOARD";
    flipVertical = false;
    delay = 1.5;
    nrFrames = 25;
    aspectRatio = 1.0;
    zeroTangentDist = false;
    fixPrincipalPoint = true;

    UpdateFlags();
}

void CaptureCalibration::SaveXML(FileStorage& fs)
{
    if (!calibrated) return;

    time_t tm;
    time( &tm );
    struct tm *t2 = localtime( &tm );
    char buf[1024];
    strftime( buf, sizeof(buf)-1, "%c", t2 );

    fs << "CalibrationTime" << buf;

    fs << "BoardWidth"  << boardSize.width
       << "BoardHeight" << boardSize.height
       << "Sidelen"         << squareSize
       << "Pattern" << patternString
       << "FrameCount" << nrFrames
       << "AspectRatio" << aspectRatio
       << "AssumeZeroTangentialDistortion" << zeroTangentDist
       << "FixPrincipalPointAtTheCenter" << fixPrincipalPoint
       << "FlipAroundHorizontalAxis" << flipVertical
       << "AnalysisTimestep" << delay;

    fs << "ImageWidth" << capture->width;
    fs << "ImageHeight" << capture->height;

    if( flags & CV_CALIB_FIX_ASPECT_RATIO )
        fs << "FixAspectRatio" << aspectRatio;

    fs << "CameraMatrix" << cameraMatrix;
    fs << "DistortionCoefficients" << distCoeffs;

/*    fs << "Map1" << undistortedMap1;
    fs << "Map2" << undistortedMap2;

    fs << "AvgReprojectionError" << totalAvgErr;
    if( !reprojErrs.empty() )
        fs << "PerViewReprojectionErrors" << Mat(reprojErrs);

    if( !rvecs.empty() && !tvecs.empty() )
    {
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
        for( int i = 0; i < (int)rvecs.size(); i++ )
        {
            Mat r = bigmat(Range(i, i+1), Range(0,3));
            Mat t = bigmat(Range(i, i+1), Range(3,6));

            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
            //*.t() is MatExpr (not Mat) so we can use assignment operator
            r = rvecs[i].t();
            t = tvecs[i].t();
        }
        cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
        fs << "ExtrinsicParameters" << bigmat;
    }
*/
    // if( !imagePoints.empty() )
    // {
    //     Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
    //     for( int i = 0; i < (int)imagePoints.size(); i++ )
    //     {
    //         Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
    //         Mat imgpti(imagePoints[i]);
    //         imgpti.copyTo(r);
    //     }
    //     fs << "Image_points" << imagePtMat;
    // }
}

void CaptureCalibration::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
	calibratedImageWidth = (int)fn["ImageWidth"];
	calibratedImageHeight = (int)fn["ImageHeight"];
	Size imgSize (calibratedImageWidth, calibratedImageHeight);

	boardSize.width = (int)fn["BoardWidth"];
	boardSize.height = (int)fn["BoardHeight"];
	squareSize = (float)fn["Sidelen"];
	
	patternString = (string)fn["Pattern"];
	pattern = NOT_EXISTING;
	if (!patternString.compare("CHESSBOARD")) pattern = CHESSBOARD;
	if (!patternString.compare("CIRCLES_GRID")) pattern = CIRCLES_GRID;
	if (!patternString.compare("ASYMMETRIC_CIRCLES_GRID")) pattern = ASYMMETRIC_CIRCLES_GRID;
	if (pattern == NOT_EXISTING)
	{
	    cerr << " Inexistent camera calibration mode: " << patternString << endl;
	}
	
	nrFrames = (int)fn["FrameCount"];
	
	aspectRatio = (float)fn["FixAspectRatio"];
	
	zeroTangentDist = (int)fn["AssumeZeroTangentialDistortion"];
	fixPrincipalPoint = (int)fn["FixPrincipalPointAtCenter"];
	flipVertical = (int)fn["FlipAroundHorizontalAxis"];
	
	UpdateFlags();
	
	delay = (float)fn["AnalysisTimestep"];
	
	// now load most important calibration data
	fn["CameraMatrix"] >> cameraMatrix;
	fn["DistortionCoefficients"] >> distCoeffs;


	bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
	
//	totalAvgErr = ComputeReprojectionErrors(objectPoints, imagePoints);

	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
				getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imgSize, 1, imgSize, 0),
				imgSize, CV_16SC2, undistortedMap1, undistortedMap2);


	// fn["Map1"] >> undistortedMap1;
	// fn["Map2"] >> undistortedMap2;
	
	boardFound = false;
	calibrating = false;
	calibrated = true;
    }
}

void CaptureCalibration::CalibrationLoadXML(cv::FileNode& fn)
{
    LoadXML(fn);

    bool goodInput = true;

    if (boardSize.width <= 0 || boardSize.height <= 0)
    {
	cerr << "Invalid Board size: " << boardSize.width << " " << boardSize.height << endl;
	goodInput = false;
    }
    if (squareSize <= 10e-6)
    {
	cerr << "Invalid square size " << squareSize << endl;
	goodInput = false;
    }
    if (nrFrames <= 0)
    {
	cerr << "Invalid number of frames " << nrFrames << endl;
	goodInput = false;
    }
    
    if (capture->width != calibratedImageWidth || capture->height != calibratedImageHeight)
    {
	cerr << "Source dimensions don't match with calibration data, aborting..." << endl;
	goodInput = false;
    }


    if (!goodInput)
    {
	boardFound = false;
	calibrating = false;
	calibrated = true;
    }
}


void CaptureCalibration::Calibrate ()
{
//    cout << "Inside Calibrate, ptr " << this << endl;

    if (!calibrating)
    {
//	cout << "Leaving Calibrate (not calibrating), ptr " << this << endl;
	return;
    }

    if (firstAnalysisStep || (capture->GetTime() - lastAnalysisTime > delay))
    {
//	cout << "delay elapsed, using that frame at time " << capture->GetTime() << endl;
	lastAnalysisTime = capture->GetTime();
	firstAnalysisStep = false;
    }
    else
    {
//	cout << "Leaving Calibrate (delay not elapsed), ptr " << this << endl;
	return;
    }

    // check if frame is static
    lastFrameNumber = frameNumber;
    frameNumber = capture->GetFrameNumber();
    if (frameNumber == lastFrameNumber)
    {
//	cout << "calibrate abort, static frame" << endl;
	return;
    }

    // try to find corners in the capture image
    Mat view = capture->frame;

    // rescale image if necessary to have decent detection speed
    Mat viewScaled = view;
    float coeff = 1;
    if (view.cols >= 1000 || view.rows >= 1000)
    {
	Mat tmp;
	if (view.cols > view.rows)
	    coeff = view.cols / 1000.0;
	else 
	    coeff = view.rows / 1000.0;
	
	Size sz (view.cols / coeff, view.rows / coeff);
	resize (view, tmp, sz);
	viewScaled = tmp;
    }

    // Format input image.
    if( flipVertical ) flip(viewScaled, viewScaled, 0);

    boardFound = false;
    switch( pattern ) // Find feature points on the input format
    {
    case CHESSBOARD:
	boardFound = findChessboardCorners( viewScaled, boardSize, pointBuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
	break;
    case CIRCLES_GRID:
	boardFound = findCirclesGrid( viewScaled, boardSize, pointBuf );
	break;
    case ASYMMETRIC_CIRCLES_GRID:
	boardFound = findCirclesGrid( viewScaled, boardSize, pointBuf, CALIB_CB_ASYMMETRIC_GRID );
	break;
    }
    cout << "Analyzed image looking for a board" << endl;

    if (boardFound)                // If done with success,
    {
	analyzedFrames++;
	cout << "Found board, analyzed  " << analyzedFrames << " frames" << endl;

	// bring points back to original scale
	for (unsigned int i = 0; i < pointBuf.size(); ++i)
	{
	    pointBuf[i].x *= coeff;
	    pointBuf[i].y *= coeff;
	}
	
	// improve the found corners' coordinate accuracy for chessboard
	if( pattern == CHESSBOARD)
	{
	    Mat viewGray;
	    cvtColor(view, viewGray, COLOR_BGR2GRAY);
	    cornerSubPix( viewGray, pointBuf, Size(11,11), Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
	}
	imagePoints.push_back(pointBuf);
    }

    if (analyzedFrames >= nrFrames)
    {
	CalculateCalibration ();

	pointBuf.clear();
	calibrating = false;
	boardFound = false;
    }

//    if (analyzedFrames >= nrFrames)
//	cout << "Done calibrating... stopping" << endl;

//    cout << "Leaving Calibrate, ptr " << this << endl;
}

void CaptureCalibration::OutputHud (Mat& hud)
{
    drawChessboardCorners(capture->frame, boardSize, Mat(pointBuf), boardFound );
    boardFound = false;
}

void CaptureCalibration::Start ()
{
    firstAnalysisStep = true;
    calibrated = false;
    calibrating = true;
    analyzedFrames = 0;
    lastFrameNumber = -1;
    imagePoints.clear();
    pointBuf.clear();
}

bool CaptureCalibration::CalculateCalibration()
{
    cameraMatrix = Mat::eye(3, 3, CV_64F);

    if ( flags & CV_CALIB_FIX_ASPECT_RATIO )
        cameraMatrix.at<double>(0,0) = 1.0;

    distCoeffs = Mat::zeros(8, 1, CV_64F);

    vector<vector<Point3f> > objectPoints(1);
    CalcBoardCornerPositions(boardSize, squareSize, objectPoints[0], pattern);

    objectPoints.resize(imagePoints.size(),objectPoints[0]);

    //Find intrinsic and extrinsic camera parameters
    double rms = calibrateCamera(objectPoints, imagePoints, capture->frame.size(), cameraMatrix, distCoeffs, rvecs, tvecs, flags | CV_CALIB_FIX_K4 | CV_CALIB_FIX_K5);

//    cout << "Re-projection error reported by calibrateCamera: "<< rms << endl;

    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

    totalAvgErr = ComputeReprojectionErrors(objectPoints, imagePoints);

    initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
			    getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, capture->frame.size(), 1, capture->frame.size(), 0),
			    capture->frame.size(), CV_16SC2, undistortedMap1, undistortedMap2);

    if (ok)
	calibrated = true;

    cout << (ok ? "Calibration succeeded" : "Calibration failed") << ". avg re projection error = "  << totalAvgErr ;
    return ok;
}


void CaptureCalibration::CalcBoardCornerPositions(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType /*= Settings::CHESSBOARD*/)
{
    corners.clear();

    switch(patternType)
    {
    case CHESSBOARD:
    case CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; ++i )
            for( int j = 0; j < boardSize.width; ++j )
                corners.push_back(Point3f(float( j*squareSize ), float( i*squareSize ), 0));
        break;

    case ASYMMETRIC_CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                corners.push_back(Point3f(float((2*j + i % 2)*squareSize), float(i*squareSize), 0));
        break;
    default:
        break;
    }
}

double CaptureCalibration::ComputeReprojectionErrors( const vector<vector<Point3f> >& objectPoints, const vector<vector<Point2f> >& imagePoints)
{
    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    reprojErrs.resize(objectPoints.size());

    for( i = 0; i < (int)objectPoints.size(); ++i )
    {
        projectPoints( Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix,
                       distCoeffs, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L2);

        int n = (int)objectPoints[i].size();
        reprojErrs[i] = (float) std::sqrt(err*err/n);
        totalErr        += err*err;
        totalPoints     += n;
    }

    return std::sqrt(totalErr/totalPoints);
}

void CaptureCalibration::Undistort (Mat& frame)
{
//    Mat temp;

    if (calibrated)
    {
	Mat temp = frame.clone();
	remap(temp, frame, undistortedMap1, undistortedMap2, INTER_LINEAR);
//	undistort(temp, frame, cameraMatrix, distCoeffs);
//	capture->frame = temp;
    }

//    return temp;
}

void CaptureCalibration::Reset()
{
    calibrated = false;
    calibrating = false;
}

void CaptureCalibration::UpdateFlags()
{
    flags = 0;
    if(fixPrincipalPoint) flags |= CV_CALIB_FIX_PRINCIPAL_POINT;
    if(zeroTangentDist)   flags |= CV_CALIB_ZERO_TANGENT_DIST;
    if(aspectRatio)       flags |= CV_CALIB_FIX_ASPECT_RATIO;
}
