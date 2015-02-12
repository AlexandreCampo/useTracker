
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

void Pipeline::Reset (cv::Rect roi)
{
    // adjust roi and locate new data source, but don't delete plugins, settings etc...
    this->roi = roi;

    width = roi.width;
    height = roi.height;

    frame = parent->capture->frame(roi);
    background = parent->background(roi);
    zoneMap = parent->zoneMap(roi);
    marked = parent->marked(roi);
    labels = parent->labels(roi);
    pipelineSnapshotMarked = parent->pipelineSnapshotMarked(roi);

    // reset plugins to take into account new data
    for (auto p : plugins)
    {
	if (p) p->Reset();
    }
}
