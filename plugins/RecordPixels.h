#ifndef RECORD_PIXELS_H
#define RECORD_PIXELS_H


#include "PipelinePlugin.h"

#include <iostream>
#include <fstream>


class RecordPixels : public PipelinePlugin
{
public:

    std::string outputFilename;
    std::fstream outputStream;

    ~RecordPixels();
    void Reset();

    void Apply();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void OutputStep();
    void OpenOutput();
    void CloseOutput();
};

#endif
