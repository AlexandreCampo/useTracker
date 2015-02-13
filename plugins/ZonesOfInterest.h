#ifndef ZONES_OF_INTEREST_H
#define ZONES_OF_INTEREST_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>


class ZonesOfInterest : public PipelinePlugin
{
public:

    ZonesOfInterest();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif
