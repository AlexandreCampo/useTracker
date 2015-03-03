#ifdef ARUCO

#include "Aruco.h"

using namespace cv;
using namespace std;
using namespace aruco;


Aruco::Aruco () : PipelinePlugin()
{
}

void Aruco::Reset()
{
//    multithreaded = true;
//    SetSize(size);
//    result = Mat::zeros (pipeline->height, pipeline->width, CV_8U);

    detector.setThresholdParams (thresh1, thresh2);
    detector.setCornerRefinementMethod (MarkerDetector::SUBPIX);
//    detector.setCornerRefinementMethod (MarkerDetector::HARRIS);
    detector.setMinMaxSize (minSize,maxSize);
    detector.setThresholdMethod(thresholdMethod);
}

void Aruco::Apply()
{
    detector.detect(pipeline->frame, markers, cameraParameters, -1);

    detector.getThresholdedImage().copyTo(pipeline->marked);
}

void Aruco::OutputHud (Mat& hud)
{
    for (unsigned int i = 0; i < markers.size(); i++)
    {
        // cv::line ( hud,markers[i][0],markers[i][1],cvScalar ( 0,0,255 ),2 ,CV_AA );
        // cv::line ( hud,markers[i][1],markers[i][2],cvScalar ( 0,0,255 ),2 ,CV_AA );
        // cv::line ( hud,markers[i][2],markers[i][3],cvScalar ( 0,0,255 ),2 ,CV_AA );
        // cv::line ( hud,markers[i][3],markers[i][0],cvScalar ( 0,0,255 ),2 ,CV_AA );

	markers[i].draw(hud, Scalar(0,255,0), 1);
    }
}

void Aruco::SetMinSize(double minSize)
{
    this->minSize = minSize;
    detector.setMinMaxSize(minSize, maxSize);
}

void Aruco::SetMaxSize(double maxSize)
{
    this->maxSize = maxSize;
    detector.setMinMaxSize(minSize, maxSize);
}

void Aruco::SetThreshold1(int t1)
{
    this->thresh1 = t1;
    detector.setThresholdParams((double)thresh1,(double)thresh2);
}

void Aruco::SetThreshold2(int t2)
{
    this->thresh2 = t2;
    detector.setThresholdParams((double)thresh1,(double)thresh2);
}

void Aruco::SetThresholdMethod(int m)
{
    this->thresholdMethod = (aruco::MarkerDetector::ThresholdMethods)m;
    detector.setThresholdMethod(thresholdMethod);
}

void Aruco::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	minSize = (float)fn["MinSize"];
	maxSize = (float)fn["MaxSize"];

	thresh1 = (float)fn["Threshold1"];
	thresh2 = (float)fn["Threshold2"];
    }
}

void Aruco::SaveXML (FileStorage& fs)
{
    fs << "MinSize" << minSize;
    fs << "MaxSize" << maxSize;
    fs << "Threshold1" << thresh1;
    fs << "Threshold2" << thresh2;
}

#endif
