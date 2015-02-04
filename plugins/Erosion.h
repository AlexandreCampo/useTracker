#ifndef EROSION_H
#define EROSION_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>

class Erosion : public PipelinePlugin
{
public:

    int size = 1;
    cv::Mat structuringElement;
    cv::Mat result;

    Erosion();

    void Reset();
    void Apply();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void SetSize(int s);
};

#endif
