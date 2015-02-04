#ifndef FRAME_DIFFERENCE_H
#define FRAME_DIFFERENCE_H

#include "PipelinePlugin.h"

#include <iostream>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>

class FrameDifference : public PipelinePlugin
{
public:
    cv::Mat previousMarked;
    cv::Mat diffMarked;
    std::fstream outputStream;
    std::string outputFilename;


    FrameDifference();

    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif

