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

#include "CaptureMultiVideo.h"

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

CaptureMultiVideo::CaptureMultiVideo(vector<string> filenames) : Capture()
{
    deviceToCalibrate = 0;
    stitched = false;
    stitching = false;
    adjustBrightness = false;

    if (Open(filenames))
	type = MULTI_VIDEO;
}

CaptureMultiVideo::CaptureMultiVideo(FileNode& fn) : Capture()
{
    deviceToCalibrate = 0;
    stitched = false;
    stitching = false;
    adjustBrightness = false;

    LoadXML(fn);
    if (Open())
	type = MULTI_VIDEO;
}

CaptureMultiVideo::~CaptureMultiVideo()
{
    for (auto c : subcaptures)
	delete (c);
}

string CaptureMultiVideo::GetName()
{
    string str = "Multi Video";
    return str;
}

bool CaptureMultiVideo::Open (vector<string> filenames)
{
    if (filenames.size() == 0) 
	return false;

    for (unsigned int i = 0; i < filenames.size(); i++)
    {
	CaptureVideo* c = new CaptureVideo(filenames[i]);

	// if one subdevice cannot be opened, clear all devices and return
	if (c->type != VIDEO)
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

bool CaptureMultiVideo::Open ()
{
    masterDevice = 0;

    // assume all sources have same fps
    fps = subcaptures[0]->fps;

    stitchPoints.resize(subcaptures.size());
    bool frameEmpty = false;
   
    if (!stitched)
    {
	ResetStitching();

	for (unsigned int i = 0; i < subcaptures.size(); i++)
	{
	    frameEmpty = frameEmpty || subcaptures[i]->frame.empty();
	}
    }
    else
    {
	for (int j = subcaptures.size() - 1; j >= 0; j--)
	{
	    unsigned int i = (unsigned int) j;

	    bool available = subcaptures[i]->GetNextFrame();
	    frameEmpty = frameEmpty || !available;
	}
    }

    // produce first frame
    if (!frameEmpty)
	Merge();

    return (!frameEmpty);
}

void CaptureMultiVideo::Close ()
{

}

bool CaptureMultiVideo::GetNextFrame ()
{
    bool frameEmpty = false;

    for (int j = subcaptures.size() - 1; j >= 0; j--)
    {
	unsigned int i = (unsigned int) j;

	bool available = subcaptures[i]->GetNextFrame();
	frameEmpty = frameEmpty || !available;
    }

    Merge();

    // at least one subcapture did not return a frame
    return !frameEmpty;
}

wxLongLong CaptureMultiVideo::GetNextFrameSystemTime()
{
    return subcaptures[masterDevice]->GetNextFrameSystemTime();
}

void CaptureMultiVideo::Stop()
{
    for (auto c : subcaptures)
	c->Stop();

    Merge();
}

void CaptureMultiVideo::Pause()
{
    for (auto c : subcaptures)
	c->Pause();
}

void CaptureMultiVideo::Play()
{
    for (auto c : subcaptures)
	c->Play();
}

bool CaptureMultiVideo::GetFrame (double time)
{
    bool frameEmpty = false;

    for (int j = subcaptures.size() - 1; j >= 0; j--)
    {
	unsigned int i = (unsigned int) j;

	bool available = subcaptures[i]->GetFrame(time);
	frameEmpty = frameEmpty || !available;
    }

    Merge();

    // at least one subcapture did not return a frame
    return !frameEmpty;
}

bool CaptureMultiVideo::GetPreviousFrame()
{
    bool frameEmpty = false;

    for (int j = subcaptures.size() - 1; j >= 0; j--)
    {
	unsigned int i = (unsigned int) j;

	bool available = subcaptures[i]->GetPreviousFrame();
	frameEmpty = frameEmpty || !available;
    }

    Merge();

    return !frameEmpty;
}

bool CaptureMultiVideo::SetFrameNumber(long frameNumber)
{
    bool frameEmpty = false;

    for (int j = subcaptures.size() - 1; j >= 0; j--)
    {
	unsigned int i = (unsigned int) j;

	bool available = subcaptures[i]->SetFrameNumber(frameNumber);
	frameEmpty = frameEmpty || !available;
    }

    Merge();

    return !frameEmpty;
}


long CaptureMultiVideo::GetFrameNumber ()
{
    return subcaptures[masterDevice]->frameNumber;
}

long CaptureMultiVideo::GetFrameCount ()
{
    unsigned long minframes = (unsigned long) -1;

    for (unsigned int i = 0; i < subcaptures.size(); i++)
    {
	unsigned long fc = (unsigned long) subcaptures[i]->GetFrameCount();
	if (fc < minframes)
	    minframes = fc;
    }

    return minframes;
}

void CaptureMultiVideo::SetSpeedFaster(int speed)
{
    for (unsigned int i = 0; i < subcaptures.size(); i++)
	subcaptures[i]->SetSpeedFaster(speed);
}

void CaptureMultiVideo::SetSpeedSlower(int speed)
{
    for (unsigned int i = 0; i < subcaptures.size(); i++)
	subcaptures[i]->SetSpeedSlower(speed);
}

double CaptureMultiVideo::GetTime()
{
    return subcaptures[masterDevice]->GetTime();
}

void CaptureMultiVideo::SaveXML(FileStorage& fs)
{
    fs << "Type" << "multiVideo";
    fs << "AdjustBrightness" << adjustBrightness;
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

void CaptureMultiVideo::LoadXML(FileNode& fn)
{
    LoadXML(fn, false);
}

void CaptureMultiVideo::LoadXML(FileNode& fn, bool stitchingOnly)
{
    if (!fn.empty())
    {
	FileNode fn2 = fn["Subdevices"];
	if (!fn2.empty())
	{
	    int sc = 0;
	    FileNodeIterator it = fn2.begin(), it_end = fn2.end();
	    for (; it != it_end; ++it)
	    {
		FileNode fn3 = (*it);

		if (stitchingOnly)
		{
		    FileNode calibNode = fn3 ["Calibration"];
		    if (!calibNode.empty())
		    {
			subcaptures[sc]->calibration.LoadXML (calibNode);
		    }		
		    sc++;
		}
		else
		{
		    CaptureVideo* capture = new CaptureVideo(fn3);
		    subcaptures.push_back(capture);
		}
	    }
	}

	// load auto brightness flag
	adjustBrightness = (int)fn["AdjustBrightness"];

	// if we need to adjust brightness
	if (adjustBrightness)
	{
	    for (unsigned int i = 0; i < subcaptures.size(); i++)
	    {
		cv::Scalar b;
		CalculateAverageBrightness(subcaptures[i], b);
		targetBrightness.push_back(b);
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

    // load first frame
    Stop();
}



// calibration methods

void CaptureMultiVideo::SetDeviceToCalibrate(int d)
{
    deviceToCalibrate = d;
}

int CaptureMultiVideo::GetDeviceToCalibrate()
{
    return deviceToCalibrate;
}

unsigned int CaptureMultiVideo::GetDeviceCount()
{
    return subcaptures.size();
}

void CaptureMultiVideo::CalibrationLoadXML (cv::FileNode& fn)
{
    subcaptures[deviceToCalibrate]->CalibrationLoadXML (fn);
}

void CaptureMultiVideo::CalibrationSaveXML (cv::FileStorage& fs)
{
    subcaptures[deviceToCalibrate]->CalibrationSaveXML (fs);
}

void CaptureMultiVideo::CalibrationStart ()
{
    subcaptures[deviceToCalibrate]->CalibrationStart();
}

void CaptureMultiVideo::CalibrationReset ()
{
    subcaptures[deviceToCalibrate]->CalibrationReset();
}

cv::Mat CaptureMultiVideo::CalibrationGetFrame()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetFrame();
}

void CaptureMultiVideo::CalibrationOutputHud(cv::Mat& hud)
{
    subcaptures[deviceToCalibrate]->CalibrationOutputHud(hud);
}

void CaptureMultiVideo::Calibrate()
{
    subcaptures[deviceToCalibrate]->Calibrate();
}

void CaptureMultiVideo::CalibrationSetBoardType(int type)
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

void CaptureMultiVideo::CalibrationSetBoardWidth(int w)
{
    subcaptures[deviceToCalibrate]->CalibrationSetBoardWidth(w);
}

void CaptureMultiVideo::CalibrationSetBoardHeight(int h)
{
    subcaptures[deviceToCalibrate]->CalibrationSetBoardHeight(h);
}

void CaptureMultiVideo::CalibrationSetSquareSize(int size)
{
    subcaptures[deviceToCalibrate]->CalibrationSetSquareSize(size);
}

void CaptureMultiVideo::CalibrationSetFramesCount(int framesCount)
{
    subcaptures[deviceToCalibrate]->CalibrationSetFramesCount(framesCount);
}

void CaptureMultiVideo::CalibrationSetAspect(float aspect)
{
    subcaptures[deviceToCalibrate]->CalibrationSetAspect(aspect);
}

void CaptureMultiVideo::CalibrationSetFrameDelay (float delay)
{
    subcaptures[deviceToCalibrate]->CalibrationSetFrameDelay(delay);
}

void CaptureMultiVideo::CalibrationSetZeroTangentDist(bool zeroTangentDist)
{
    subcaptures[deviceToCalibrate]->CalibrationSetZeroTangentDist(zeroTangentDist);
}

void CaptureMultiVideo::CalibrationSetFixPrincipalPoint(bool fixPrincipalPoint)
{
    subcaptures[deviceToCalibrate]->CalibrationSetFixPrincipalPoint (fixPrincipalPoint);
}

void CaptureMultiVideo::CalibrationSetFlipVertical(bool flipVertical)
{
    subcaptures[deviceToCalibrate]->CalibrationSetFlipVertical (flipVertical);
}

// default stuff only for multi captures
std::string CaptureMultiVideo::GetDeviceName(unsigned int d)
{
    return subcaptures[d]->GetName();
}

int CaptureMultiVideo::CalibrationGetBoardType()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetBoardType();
}

int CaptureMultiVideo::CalibrationGetBoardWidth()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetBoardWidth();
}

int CaptureMultiVideo::CalibrationGetBoardHeight()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetBoardHeight();
}

int CaptureMultiVideo::CalibrationGetSquareSize()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetSquareSize();
}

int CaptureMultiVideo::CalibrationGetFramesCount()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetFramesCount();
}

int CaptureMultiVideo::CalibrationGetAspectNum()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetAspectNum();
}

int CaptureMultiVideo::CalibrationGetAspectDen()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetAspectDen();
}

float CaptureMultiVideo::CalibrationGetFrameDelay()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetFrameDelay();
}

bool CaptureMultiVideo::CalibrationGetZeroTangentDist()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetZeroTangentDist();
}

bool CaptureMultiVideo::CalibrationGetFixPrincipalPoint()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetFixPrincipalPoint();
}

bool CaptureMultiVideo::CalibrationGetFlipVertical()
{
    return subcaptures[deviceToCalibrate]->CalibrationGetFlipVertical();
}

bool CaptureMultiVideo::TryToStitch()
{
    return false;
}

void CaptureMultiVideo::ResetStitching()
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

bool CaptureMultiVideo::Stitch()
{
    bool boardFound0 = false;
    bool boardFound1 = false;

    std::vector<cv::Point2f> points0;
    std::vector<cv::Point2f> points1;

    // rescale image if necessary to have decent detection speed
    Mat frame0 = subcaptures[0]->frame;
    Mat frame0Scaled = frame0;
    float coeff0 = 1;
    if (frame0.cols >= 1000 || frame0.rows >= 1000)
    {
	Mat tmp;
	if (frame0.cols > frame0.rows)
	    coeff0 = frame0.cols / 1000.0;
	else 
	    coeff0 = frame0.rows / 1000.0;
	
	Size sz (frame0.cols / coeff0, frame0.rows / coeff0);
	resize (frame0, tmp, sz);
	frame0Scaled = tmp;
    }

    Mat frame1 = subcaptures[1]->frame;
    Mat frame1Scaled = frame1;
    float coeff1 = 1;
    if (frame1.cols >= 1000 || frame1.rows >= 1000)
    {
	Mat tmp;
	if (frame1.cols > frame1.rows)
	    coeff1 = frame1.cols / 1000.0;
	else 
	    coeff1 = frame1.rows / 1000.0;
	
	Size sz (frame1.cols / coeff1, frame1.rows / coeff1);
	resize (frame1, tmp, sz);
	frame1Scaled = tmp;
    }

    boardFound0 = findChessboardCorners( frame0Scaled, subcaptures[0]->calibration.boardSize, points0, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
    boardFound1 = findChessboardCorners( frame1Scaled, subcaptures[0]->calibration.boardSize, points1, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

    if (!boardFound0 || !boardFound1)
    {
	cout << "Stitching failed, did not detect chessboard in some pictures " << boardFound0 << " " << boardFound1 << endl;
	return false;
    }

    // bring points back to original scale
    for (unsigned int i = 0; i < points0.size(); ++i)
    {
	points0[i].x *= coeff0;
	points0[i].y *= coeff0;
    }
    for (unsigned int i = 0; i < points1.size(); ++i)
    {
	points1[i].x *= coeff1;
	points1[i].y *= coeff1;
    }

    // improve the found corners' coordinate accuracy for chessboard
    Mat viewGray0;
    cvtColor(subcaptures[0]->frame, viewGray0, COLOR_BGR2GRAY);
    cornerSubPix( viewGray0, points0, Size(11,11), Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

    Mat viewGray1;
    cvtColor(subcaptures[1]->frame, viewGray1, COLOR_BGR2GRAY);
    cornerSubPix( viewGray1, points1, Size(11,11), Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

    // reorder points if they don't go left to right, top to bottom (findChessBoardCorners bug in opencv ?)
    unsigned int bwidth = subcaptures[0]->calibration.boardSize.width;

    if (points0[0].x > points0[bwidth].x)
    {
	reverse (points0.begin(), points0.end());
    }
    if (points1[0].x > points1[bwidth].x)
    {
	reverse (points1.begin(), points1.end());
    }

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

    H = findHomography( stitchPoints[1], stitchPoints[0]);
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

  // now calculate copy masks...
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

void CaptureMultiVideo::CalculateAverageBrightness(Capture* capture, cv::Scalar& brightness)
{
    int width = capture->width;
    int height = capture->height;
    Mat cumulated = Mat::zeros(height, width, CV_32SC3);

    float endTime = ((double)GetFrameCount()) / fps;

    cerr << "Calculating average luminosity for capture " << capture << endl;

    capture->Stop();
    capture->Play();

    // scan X frames
    unsigned int readCount = 0;
    unsigned int framesCount = 10;
    float intervalTime = endTime / framesCount;

    cv::Scalar sum;
    while (readCount < framesCount)
    {
	capture->GetFrame (intervalTime * readCount);

	double msgTime = capture->GetTime();
    	cerr << "using frame at time " << msgTime << endl;

    	if (capture->frame.empty())
    	    break;

	cv::Scalar s = cv::sum(capture->frame);
	sum = sum + s;

    	readCount++;
    }

    capture->Stop();

    brightness = sum / (double)(framesCount * width * height);
}


void CaptureMultiVideo::AdjustBrightness (int sc)
{
    Capture* capture = subcaptures[sc];

    // calculate average brightness
    cv::Scalar brightness = cv::sum(capture->frame);
    brightness = brightness / (double)(capture->width * capture->height);
    
    // get difference to target
    cv::Scalar diff = targetBrightness[sc] - brightness;

    capture->frame = capture->frame + diff;
}

void CaptureMultiVideo::Merge()
{
    for (int j = subcaptures.size() - 1; j >= 0; j--)
    {
	unsigned int i = (unsigned int) j;

	if (adjustBrightness)
	    AdjustBrightness (i);

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
}
