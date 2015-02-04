
#include "Pipeline.h"

#include "ImageProcessingEngine.h"

Pipeline::Pipeline (ImageProcessingEngine* ipEngine, cv::Rect roi)
{
    this->parent = ipEngine;
    this->roi = roi;

    width = roi.width;
    height = roi.height;

    frame = ipEngine->capture->frame(roi);
    background = ipEngine->background(roi);
    zoneMap = ipEngine->zoneMap(roi);
    marked = ipEngine->marked(roi);
    labels = ipEngine->labels(roi);
    pipelineSnapshotMarked = ipEngine->pipelineSnapshotMarked(roi);
}

Pipeline::~Pipeline()
{
    for (auto p : plugins)
    {
	if (p) delete (p);
    }
}
