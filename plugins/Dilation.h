#ifndef DILATION_H
#define DILATION_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


class Dilation : public PipelinePlugin
{
public:

    int size = 1;
    cv::Mat structuringElement;
    cv::Mat result;

    Dilation();

    void Reset();
    void Apply();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void SetSize(int s);
};

#endif
