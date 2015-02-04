#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include "PipelinePlugin.h"

#include <queue>
#include <opencv2/imgproc/imgproc.hpp>

class MovingAverage : public PipelinePlugin
{
public:
    std::queue<cv::Mat> frames;
    cv::Mat movingAverage;
    cv::Mat framesSum;
    cv::Mat oldFrame;
    unsigned int length = 10;
    unsigned int threshold = 5;

    MovingAverage();
    ~MovingAverage();

    void Reset();
    void Apply();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void SetLength(unsigned int l);
    void SetThreshold(unsigned int t);

};

#endif

