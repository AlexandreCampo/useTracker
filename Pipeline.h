#ifndef PIPELINE_H
#define PIPELINE_H


// the elements may be relocalized inside plugins
#include <opencv2/highgui/highgui.hpp>


class ImageProcessingEngine;
class PipelinePlugin;

// convention about zones for all plugins
#define ZONE_VISIBLE 1
#define ZONE_OUT 0

class Pipeline
{
public:
    ImageProcessingEngine* parent;
    std::vector<PipelinePlugin*> plugins;

    // data shared across all plugins
    cv::Rect roi;
    int width;
    int height;

    cv::Mat frame;

    cv::Mat background;
    cv::Mat zoneMap;
    cv::Mat marked;
    cv::Mat labels;
    cv::Mat pipelineSnapshotMarked;

    Pipeline (ImageProcessingEngine* ipEngine, cv::Rect roi);
    ~Pipeline ();
    void Reset (cv::Rect roi);
};

#endif
