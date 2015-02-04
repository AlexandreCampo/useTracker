#ifndef EXTRACT_BLOBS_H
#define EXTRACT_BLOBS_H

#include "PipelinePlugin.h"

#include <iostream>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>


class ExtractBlobs : public PipelinePlugin
{
public:

    struct Segment
    {
    Segment(){};
	Segment(unsigned int min) {this->min = min;}
	unsigned int min, max, sumX, sumY, size;
	unsigned int blobIdx;
    };

    unsigned int minSize = 10;

    std::string outputFilename;
    std::fstream outputStream;

    bool recordLabels = false;

    ~ExtractBlobs();
    void Reset();
    
    void Apply();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
    void OutputHud (cv::Mat& hud);

    void OutputStep();
    void OpenOutput();
    void CloseOutput();
};

#endif

