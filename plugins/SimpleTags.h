#ifndef SIMPLE_TAGS_H
#define SIMPLE_TAGS_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>


class SimpleTags : public PipelinePlugin
{
public:

    cv::Mat pattern;
    std::vector<int> p;
    int pwidth = 100;
    int pheight = 20;

    std::string outputFilename;
    std::fstream outputStream;

    SimpleTags();
    ~SimpleTags();

    void Reset();
    void Apply();
    void OutputHud (cv::Mat& hud);
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void OutputStep();
    void OpenOutput();
    void CloseOutput();

    void SetTagDimensions(int width, int height);
};


#endif
