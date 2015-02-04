#ifndef COLOR_SEGMENTATION_H
#define COLOR_SEGMENTATION_H

#include "PipelinePlugin.h"

#include <opencv2/opencv.hpp>

class ColorSegmentation : public PipelinePlugin
{
public:
    enum Type {BGR,HSV};

    Type type = HSV;

    cv::Mat frameHSV;
    cv::Mat marked2;
    cv::Vec3b minBGR = {0,0,0};
    cv::Vec3b maxBGR = {255,255,255};
    cv::Vec3b minHSV = {0,0,0};
    cv::Vec3b maxHSV = {180,255,255};

    cv::Mat pixelSrc;
    cv::Mat pixelDst;
    cv::Mat pixelRes;
    cv::Mat pixelRes2;

    ColorSegmentation();
    void Reset();
    void Apply();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif
