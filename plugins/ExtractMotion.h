#ifndef EXTRACT_MOTION_H
#define EXTRACT_MOTION_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>


class ExtractMotion : public PipelinePlugin
{
public:

    int threshold = 20;

    cv::Mat diff;
    cv::Mat sum;
    cv::Mat marked2;
//    Mat background;

    ExtractMotion();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif
