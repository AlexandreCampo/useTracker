#ifndef SIMPLE_TAGS_H
#define SIMPLE_TAGS_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>


class SimpleTags : public PipelinePlugin
{
public:

    int pwidth = 100;
    int pheight = 25;

    SimpleTags();

    void Reset();
    void Apply();
    void OutputHud (cv::Mat& hud);
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};


#endif
