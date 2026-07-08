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

// number of future frames this plugin needs prefetched ahead of the current
// frame (0 = none). The engine keeps the decoder this many frames ahead so
// the plugin can read future frames from the shared buffer.
virtual int PrefetchAhead () { return 0; }

// how many frames this plugin's result lags the frame it processes (0 = none).
// A centered temporal-mask plugin processes frame t but its output corresponds
// to frame t-L; the engine presents the frame L behind the pipeline so the
// (de-lagged) result aligns with the displayed image.
virtual int OutputLatency () { return 0; }

virtual void Apply () = 0;
virtual void OpenOutput () {};
virtual void CloseOutput () {};
virtual void OutputStep () {};
virtual void OutputHud (cv::Mat& hud) {};

virtual void LoadXML(cv::FileNode& fn) {};
virtual void SaveXML(cv::FileStorage& fs) {};
};

#endif
