#ifdef ARUCO

#ifndef ARUCO_H
#define ARUCO_H

#include "PipelinePlugin.h"

#include <iostream>
#include <fstream>

#include <aruco/aruco.h>
#include <opencv2/imgproc/imgproc.hpp>

class Aruco : public PipelinePlugin
{
public:

    aruco::MarkerDetector detector;
    std::vector<aruco::Marker> markers;
    aruco::CameraParameters cameraParameters;

    double minSize = 0.001;
    double maxSize = 0.1;

    int thresh1 = 19;
    int thresh2 = 7;

    aruco::MarkerDetector::ThresholdMethods thresholdMethod = aruco::MarkerDetector::ADPT_THRES;

    std::string outputFilename;
    std::fstream outputStream;

    Aruco();
    ~Aruco();

    void Reset();
    void Apply();
    void OutputHud (cv::Mat& hud);
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void OutputStep();
    void OpenOutput();
    void CloseOutput();

void SetMinSize(double minSize);
void SetMaxSize(double maxSize);
void SetThreshold1(int t1);
void SetThreshold2(int t2);
void SetThresholdMethod (int m);

//    void SetSize(int s);
};



#endif

#endif
