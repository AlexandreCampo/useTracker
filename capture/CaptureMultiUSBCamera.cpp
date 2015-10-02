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

#include "CaptureMultiUSBCamera.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>

#include <wx/time.h>

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/stitching/stitcher.hpp>

#include "Utils.h"

using namespace std;
using namespace cv;

CaptureMultiUSBCamera::CaptureMultiUSBCamera(vector<int> devices) : Capture()
{
    deviceToCalibrate = 0;
    stitched = false;
    stitching = false;

    if (Open(devices))
	type = MULTI_USB_CAMERA;
}

CaptureMultiUSBCamera::CaptureMultiUSBCamera(FileNode& fn) : Capture()
{
    deviceToCalibrate = 0;
    stitched = false;
    stitching = false;

    LoadXML(fn);
    if (Open())
	type = MULTI_USB_CAMERA;
}

CaptureMultiUSBCamera::~CaptureMultiUSBCamera()
{
    for (auto c : subcaptures)
	delete (c);
}

string CaptureMultiUSBCamera::GetName()
{
    string str = "Multi USB Camera devices";
    return str;
}

bool CaptureMultiUSBCamera::Open (vector<int> devices)
{
    for (unsigned int i = 0; i < devices.size(); i++)
    {
	CaptureUSBCamera* c = new CaptureUSBCamera(devices[i]);

	// if one subdevice cannot be opened, clear all devices and return
	if (c->type != USB_CAMERA)
	{
	    for (unsigned int j = 0; j < subcaptures.size(); j++)
		delete subcaptures[j];

	    subcaptures.clear();
	    return false;
	}

	subcaptures.push_back(c);
    }

    // continue setting things up after creating subcaptures
    return Open ();
}

bool CaptureMultiUSBCamera::Open ()
{
    float minFPS = 1000000;
    masterDevice = 0;
    for (unsigned int i = 0; i < subcaptures.size(); i++)
    {
	CaptureUSBCamera* c = subcaptures[i];

	// slowest device drives the whole metacapture
	if (minFPS > c->fps)
	{
	    minFPS = c->fps;
	    masterDevice = i;
	}
    }
    fps = minFPS;

    stitchPoints.resize(subcaptures.size());
    bool frameMissing = false;
	
    if (!stitched)
    {
	ResetStitching();
	
	for (unsigned int i = 0; i < subcaptures.size(); i++)
	{
	    frameMissing = frameMissing || subcaptures[i]->frame.empty();
	}
	
	// produce first frame
	if (!frameMissing)
	{
	    for (unsigned int i = 0; i < subcaptures.size(); i++)
	    {
		subcaptures[i]->frame.copyTo(frame(rects[i]));
	    }
	}
    }
    else
    {
	for (int j = subcaptures.size() - 1; j >= 0; j--)
	{
	    unsigned int i = (unsigned int) j;
	    
	    subcaptures[i]->GetNextFrame();
	    frameMissing = frameMissing || subcaptures[i]->frame.empty();
	
	    Mat tmp;
	    warpPerspective(subcaptures[i]->frame, tmp, homographies[i], frame.size());
	    tmp.copyTo(frame, stitchMasks[i]);
	}
    }

    return (!frameMissing);
}

void CaptureMultiUSBCamera::Close ()
{

}

bool CaptureMultiUSBCamera::GetNextFrame ()
{
    Mat previousFrame = frame;

    bool frameEmpty = false;

    for (int j = subcaptures.size() - 1; j >= 0; j--)
    {
	unsigned int i = (unsigned int) j;

	subcaptures[i]->GetNextFrame();
	frameEmpty = frameEmpty || subcaptures[i]->frame.empty();

	if (stitched)
	{
	    Mat tmp;
	    warpPerspective(subcaptures[i]->frame, tmp, homographies[i], frame.size());
	    tmp.copyTo(frame, stitchMasks[i]);
	}
	else
	{
	    subcaptures[i]->frame.copyTo(frame(rects[i]));
	}
    }

    // at least one subcapture did not return a frame
    if (frameEmpty)
    {
	frame = previousFrame;
	return false;
    }
    return true;
}

wxLongLong CaptureMultiUSBCamera::GetNextFrameSystemTime()
{
    return subcaptures[masterDevice]->GetNextFrameSystemTime();
}

void CaptureMultiUSBCamera::Stop()
{
    for (auto c : subcaptures)
	c->Stop();
}

void CaptureMultiUSBCamera::Pause()
{
    for (auto c : subcaptures)
	c->Pause();
}

void CaptureMultiUSBCamera::Play()
{
    for (auto c : subcaptures)
	c->Play();
}

bool CaptureMultiUSBCamera::GetFrame (double time)
{
    while (InternalGetTime() < time) this_thread::sleep_for(chrono::milliseconds(10));
    Mat previousFrame = frame;

    // take several frames otherwise we get an old buffered frame
    for (int j = 0; j < 4; j++)
    {
	for (unsigned int i = 0; i < subcaptures.size(); i++)
	{
	    subcaptures[i]->GetNextFrame();
	}
    }

    bool frameEmpty = false;
    for (unsigned int i = subcaptures.size() - 1; i >= 0; i--)
    {
	frameEmpty = frameEmpty || subcaptures[i]->frame.empty();

	if (stitched)
	{
	    Mat tmp;
	    warpPerspective(subcaptures[i]->frame, tmp, homographies[i], frame.size());
	    tmp.copyTo(frame, stitchMasks[i]);
	}
	else
	{
	    subcaptures[i]->frame.copyTo(frame(rects[i]));
	}
    }

    // at least one subcapture did not return a frame
    if (frameEmpty)
    {
	frame = previousFrame;
	return false;
    }
    return true;
}

long CaptureMultiUSBCamera::GetFrameNumber ()
{
    return subcaptures[masterDevice]->frameNumber;
}

long CaptureMultiUSBCamera::GetFrameCount ()
{
    return 1;
}

double CaptureMultiUSBCamera::GetTime()
{
    return subcaptures[masterDevice]->GetTime();
}

wxLongLong CaptureMultiUSBCamera::InternalGetTime()
{
    return subcaptures[masterDevice]->InternalGetTime();
}

void CaptureMultiUSBCamera::SaveXML(FileStorage& fs)
{
    fs << "Type" << "multiUSBcamera";
    fs << "Subdevices" << "{";

    for (unsigned int i = 0; i < subcaptures.size(); i++)
    {
	fs << std::string("Subdevice_") + std::to_string(i) << "{";

	subcaptures[i]->SaveXML(fs);

	fs << "}";
    }
    fs << "}";

    // save stitching params
    if (stitched)
    {
	fs << "Stitching" << "{";

	for (unsigned int i = 0; i < subcaptures.size(); i++)
	{
	    vector<int> compression_params;
	    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	    compression_params.push_back(9);
	    vector <unsigned char> buffer;
	    imencode (".png", stitchMasks[i], buffer, compression_params);

	    fs << std::string("Homography_") + std::to_string(i) << homographies[i];
	    fs << std::string("StitchMask_") + std::to_string(i) << buffer;
	}
	fs << "}";
    }
}

void CaptureMultiUSBCamera::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
	FileNode fn2 = fn["Subdevices"];
	if (!fn2.empty())
	{
	    FileNodeIterator it = fn2.begin(), it_end = fn2.end();
	    for (; it != it_end; ++it)
	    {
		FileNode fn3 = (*it);
		CaptureUSBCamera* capture = new CaptureUSBCamera(fn3);
		subcaptures.push_back(capture);
	    }
	}

	// load stitching
	fn2 = fn["Stitching"];
	if (!fn2.empty())
	{
	    homographies.clear();
	    stitchMasks.clear();

	    for (unsigned int i = 0; i < subcaptures.size(); i++)
	    {
		cv::String str1 = std::string("Homography_") + std::to_string(i);
		cv::String str2 = std::string("StitchMask_") + std::to_string(i);

		Mat h;
		vector<unsigned char> buffer;

		fn2[str1] >> h;
		fn2[str2] >> buffer;

		Mat mask = imdecode (buffer, IMREAD_GRAYSCALE);

		homographies.push_back(h);
		stitchMasks.push_back(mask);
	    }

	    height = stitchMasks[0].rows;
	    width = stitchMasks[0].cols;
	    frame = Mat::zeros(height, width, CV_8UC3);

	    stitched = true;
	}
	else
	{
	    stitched = false;
	}
    }
}



// calibration methods

void CaptureMultiUSBCamera::SetDeviceToCalibrate(int d)
{
    deviceToCalibrate = d;
}

int CaptureMultiUSBCamera::GetDeviceToCalibrate()
{
    return deviceToCalibrate;
}

unsigned int CaptureMultiUSBCamera::GetDeviceCount()
{
    return subcaptures.size();
}

void CaptureMultiUSBCamera::CalibrationLoadXML (cv::FileNode& fn)
{
    subcaptures[deviceToCalibrate]->CalibrationLoadXML (fn);
}

void CaptureMultiUSBCamera::CalibrationSaveXML (cv::FileStorage& fs)
{
    subcaptures[deviceToCalibrate]->CalibrationSaveXML (fs);
}

void CaptureMultiUSBCamera::CalibrationStart ()
{
    subcaptures[deviceToCalibrate]->CalibrationStart();
}

void CaptureMultiUSBCamera::CalibrationReset ()
{
    subcaptures[deviceToCalibrate]->CalibrationReset();
}

cv::Mat CaptureMultiUSBCamera::CalibrationGetFrame()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetFrame();
}

void CaptureMultiUSBCamera::CalibrationOutputHud(cv::Mat& hud)
{
    subcaptures[deviceToCalibrate]->CalibrationOutputHud(hud);
}

void CaptureMultiUSBCamera::Calibrate()
{
    subcaptures[deviceToCalibrate]->Calibrate();
}

void CaptureMultiUSBCamera::CalibrationSetBoardType(int type)
{
    subcaptures[deviceToCalibrate]->CalibrationSetBoardType(type);
    /*if (type == 0)
    {
	subcaptures[deviceToCalibrate]->pattern = CaptureCalibration::CHESSBOARD;
	subcaptures[deviceToCalibrate]->patternString = "CHESSBOARD";
    }
    else if (type == 1)
    {
	subcaptures[deviceToCalibrate]->pattern = CaptureCalibration::CIRCLES_GRID;
	subcaptures[deviceToCalibrate]->patternString = "CIRCLES_GRID";
    }
    else if (type == 2)
    {
	subcaptures[deviceToCalibrate]->pattern = CaptureCalibration::ASYMMETRIC_CIRCLES_GRID;
	subcaptures[deviceToCalibrate]->patternString = "ASYMMETRIC_CIRCLES_GRID";
    }*/
}

void CaptureMultiUSBCamera::CalibrationSetBoardWidth(int w)
{
    subcaptures[deviceToCalibrate]->CalibrationSetBoardWidth(w);
}

void CaptureMultiUSBCamera::CalibrationSetBoardHeight(int h)
{
    subcaptures[deviceToCalibrate]->CalibrationSetBoardHeight(h);
}

void CaptureMultiUSBCamera::CalibrationSetSquareSize(int size)
{
    subcaptures[deviceToCalibrate]->CalibrationSetSquareSize(size);
}

void CaptureMultiUSBCamera::CalibrationSetFramesCount(int framesCount)
{
    subcaptures[deviceToCalibrate]->CalibrationSetFramesCount(framesCount);
}

void CaptureMultiUSBCamera::CalibrationSetAspect(float aspect)
{
    subcaptures[deviceToCalibrate]->CalibrationSetAspect(aspect);
}

void CaptureMultiUSBCamera::CalibrationSetZeroTangentDist(bool zeroTangentDist)
{
    subcaptures[deviceToCalibrate]->CalibrationSetZeroTangentDist(zeroTangentDist);
}

void CaptureMultiUSBCamera::CalibrationSetFixPrincipalPoint(bool fixPrincipalPoint)
{
    subcaptures[deviceToCalibrate]->CalibrationSetFixPrincipalPoint (fixPrincipalPoint);
}

void CaptureMultiUSBCamera::CalibrationSetFlipVertical(bool flipVertical)
{
    subcaptures[deviceToCalibrate]->CalibrationSetFlipVertical (flipVertical);
}

// default stuff only for multi captures
std::string CaptureMultiUSBCamera::GetDeviceName(unsigned int d)
{
    return std::to_string(subcaptures[d]->device);
}

int CaptureMultiUSBCamera::CalibrationGetBoardType()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetBoardType();
}

int CaptureMultiUSBCamera::CalibrationGetBoardWidth()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetBoardWidth();
}

int CaptureMultiUSBCamera::CalibrationGetBoardHeight()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetBoardHeight();
}

int CaptureMultiUSBCamera::CalibrationGetSquareSize()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetSquareSize();
}

int CaptureMultiUSBCamera::CalibrationGetFramesCount()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetFramesCount();
}

int CaptureMultiUSBCamera::CalibrationGetAspectNum()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetAspectNum();
}

int CaptureMultiUSBCamera::CalibrationGetAspectDen()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetAspectDen();
}

float CaptureMultiUSBCamera::CalibrationGetFrameDelay()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetFrameDelay();
}

bool CaptureMultiUSBCamera::CalibrationGetZeroTangentDist()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetZeroTangentDist();
}

bool CaptureMultiUSBCamera::CalibrationGetFixPrincipalPoint()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetFixPrincipalPoint();
}

bool CaptureMultiUSBCamera::CalibrationGetFlipVertical()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetFlipVertical();
}

bool CaptureMultiUSBCamera::TryToStitch()
{
    return false;
}

void CaptureMultiUSBCamera::ResetStitching()
{
    height = 0;
    width = 0;
    for (unsigned int i = 0; i < subcaptures.size(); i++)
    {
	int w = subcaptures[i]->width;
	int h = subcaptures[i]->height;

	rects.push_back(Rect(width, 0, w, h));
	
	// calculate merged frame -> assemble side by side
	if (h > height) height = h;
	width += w;
    }

    for (unsigned int i = 0; i < stitchPoints.size(); i++)
	stitchPoints[i].clear();
    frame = Mat::zeros(height, width, CV_8UC3);

    stitched = false;
}

bool CaptureMultiUSBCamera::Stitch()
{
    bool boardFound0 = false;
    bool boardFound1 = false;

    std::vector<cv::Point2f> points0;
    std::vector<cv::Point2f> points1;

    boardFound0 = findChessboardCorners( subcaptures[0]->frame, subcaptures[0]->calibration.boardSize, points0, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
    boardFound1 = findChessboardCorners( subcaptures[1]->frame, subcaptures[0]->calibration.boardSize, points1, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

    if (!boardFound0 || !boardFound1)
    {
	cout << "Stitching failed, did not detect chessboard in some pictures" << endl;
	return false;
    }

    // improve the found corners' coordinate accuracy for chessboard
    Mat viewGray0;
    cvtColor(subcaptures[0]->frame, viewGray0, COLOR_BGR2GRAY);
    cornerSubPix( viewGray0, points0, Size(11,11), Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

    Mat viewGray1;
    cvtColor(subcaptures[1]->frame, viewGray1, COLOR_BGR2GRAY);
    cornerSubPix( viewGray1, points1, Size(11,11), Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

    // now calculate matching
    homographies.clear();
    stitchMasks.clear();
    stitched = false;

    // first homography
    Mat H = (Mat_<double> (3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
    homographies.push_back(H);

    // add points to all other points...
    stitchPoints[0].insert(stitchPoints[0].end(), points0.begin(), points0.end());
    stitchPoints[1].insert(stitchPoints[1].end(), points1.begin(), points1.end());

    H = findHomography( stitchPoints[1], stitchPoints[0], CV_RANSAC );
    homographies.push_back(H);

    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<Point2f> corners(4);
    corners[0] = cvPoint(0,0);
    corners[1] = cvPoint( subcaptures[1]->frame.cols, 0 );
    corners[2] = cvPoint( subcaptures[1]->frame.cols, subcaptures[1]->frame.rows );
    corners[3] = cvPoint( 0, subcaptures[1]->frame.rows );
    std::vector<Point2f> warpedCorners(4);

    perspectiveTransform( corners, warpedCorners, H);

    // calculate size of stitched image
    Rect rect0 = Rect (0, 0, subcaptures[0]->frame.cols, subcaptures[0]->frame.rows);
    Rect rect1 = boundingRect(warpedCorners);
    Rect stitchedRect = rect0 | rect1;

    // ensure dimensions are even numbers (necessary to record h264 movies)
    if (stitchedRect.width % 2 == 1) stitchedRect.width++;
    if (stitchedRect.height % 2 == 1) stitchedRect.height++;

    cout << "Stitched rect : " << stitchedRect.x << " " << stitchedRect.y << " ";
    cout << stitchedRect.width << " " << stitchedRect.height << endl;

    // create new frame
    width = stitchedRect.width;
    height = stitchedRect.height;

    frame = Mat::zeros(height, width, CV_8UC3);

    // calculate the 2 transformations
    // 1st capture is just moved

    float shiftx = stitchedRect.x;
    float shifty = stitchedRect.y;

    if (shifty > 0.0) shifty = 0.0;
    if (shiftx > 0.0) shiftx = 0.0;

    // modify homography to send inside the pic (translation)
    for (unsigned int i = 0; i < homographies.size(); i++)
    {
	homographies[i].at<double>(0,0) -= shiftx * homographies[i].at<double>(2,0);
	homographies[i].at<double>(0,1) -= shiftx * homographies[i].at<double>(2,1);
	homographies[i].at<double>(0,2) -= shiftx * homographies[i].at<double>(2,2);

	homographies[i].at<double>(1,0) -= shifty * homographies[i].at<double>(2,0);
	homographies[i].at<double>(1,1) -= shifty * homographies[i].at<double>(2,1);
	homographies[i].at<double>(1,2) -= shifty * homographies[i].at<double>(2,2);
    }

    // now calculate copy masks
    std::vector<cv::Mat> masks;
    masks.resize(2);
    
    // 1 create a mask of the first image
    masks[0] = Mat::zeros(frame.size(), CV_8U);
    Mat tmp0 = Mat::ones(subcaptures[0]->frame.size(), CV_8U);
    subcaptures[0]->calibration.Undistort(tmp0);
    warpPerspective(tmp0, masks[0], homographies[0], masks[0].size());

    // 2 project second image, and create mask
    masks[1] = Mat::zeros(frame.size(), CV_8U);
    Mat tmp1 = Mat::ones(subcaptures[1]->frame.size(), CV_8U);
    subcaptures[1]->calibration.Undistort(tmp1);
    warpPerspective(tmp1, masks[1], homographies[1], masks[1].size());
    
    // 3 create new accumulator and add the 2 masks so as to have different zones
    Mat accu = masks[0].clone();
    accu += masks[1];
    
    // 4 keep only intersection
    Mat overlap;
    threshold (accu, overlap, 1, 1, THRESH_BINARY);
    
    // 5 find overlap bounding box and center of mass
    long int cx = 0;
    long int cy = 0;
    int n = 0;
    vector<Point> pts;
    for (int y = 0; y < overlap.rows; y++)
    {
	unsigned char* ptr = overlap.ptr(y);
	for (int x = 0; x < overlap.cols; x++)
	{
	    if (*ptr)
	    {
		pts.push_back(Point(x,y));
		cx += x;
		cy += y;
		n++;
	    }
	    ptr++;
	}
    }
    cx /= n;
    cy /= n;
    Rect box = boundingRect(pts);
    
    // 5 define a separation line based on center of masses
    Point c0;
    c0.x = rect0.x - shiftx + rect0.width / 2;
    c0.y = rect0.y - shifty + rect0.height / 2;
    
    Point c1;
    c1 = warpedCorners[0] + warpedCorners[1] + warpedCorners[2] + warpedCorners[3];
    c1.x /= 4;
    c1.y /= 4;
    
    Point diff = c1 - c0;
    
    float A, B;
    
    // separation line has a slope != infinity
    if (diff.y != 0)
    {
	A = float(-diff.x) / float(diff.y);
	B = float(cy) - A * float(cx);
	
	// 6 scan overlap region and assign pixels to masks
	bool maskAbove = true;
	
	if (c0.y < c0.x * A + B)
	    maskAbove = false;
	
	Mat region = overlap (box);
	for (int y = 0; y < region.rows; y++)
	{
	    unsigned char* ptr = region.ptr(y);
	    for (int x = 0; x < region.cols; x++)
	    {
		if (*ptr)
		{
		    // find if pixel belongs to mask0
		    bool pixelAbove = true;
		    int px = x + box.x;
		    int py = y + box.y;
		    if (py < px * A + B)
			pixelAbove = false;
		    
		    if (maskAbove != pixelAbove)
			masks[0].at<unsigned char> (py, px) = 0;
		}
		ptr++;
	    }
	}
    }
    // separation line is vertical
    else
    {
	// 6 scan overlap region and assign pixels to masks
	bool maskLeft = true;
	
	if (c0.x > cx)
	    maskLeft = false;
	
	Mat region = overlap (box);
	for (int y = 0; y < region.rows; y++)
	{
	    unsigned char* ptr = region.ptr(y);
	    for (int x = 0; x < region.cols; x++)
	    {
		if (*ptr)
		{
		    // find if pixel belongs to mask0
		    bool pixelLeft = true;
		    if (x + box.x > cx)
			pixelLeft = false;
		    
		    if (maskLeft != pixelLeft)
			masks[0].at<unsigned char> (y + box.y, x + box.x) = 0;
		}
		ptr++;
	    }
	}
    }
    
    stitchMasks = masks; 
    stitched = true;
    
    return true;
}
