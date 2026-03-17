#ifndef PIPELINE_PLUGIN_H
#define PIPELINE_PLUGIN_H


#include <opencv2/core.hpp>
#include <string>

#include "Pipeline.h"


class PipelinePlugin
{
public:

Pipeline* pipeline;

std::string registryName;
int active;
int output;
bool multithreaded;

PipelinePlugin ()
{
    pipeline = nullptr;
    active = false;
    output = false;
    multithreaded = false;
};
virtual ~PipelinePlugin () {};

virtual void Reset (){};

virtual void Apply () = 0;
virtual void OpenOutput () {};
virtual void CloseOutput () {};
virtual void OutputStep () {};
virtual void OutputHud (cv::Mat& hud) {};

virtual void LoadXML(cv::FileNode& fn) {};
virtual void SaveXML(cv::FileStorage& fs) {};
};

#endif
