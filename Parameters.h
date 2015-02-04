
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <boost/program_options.hpp>

#include "opencv2/opencv.hpp"

class PipelineFunction;

class Parameters
{
public:

    cv::FileStorage file;
    cv::FileNode rootNode;

    bool nogui;

    std::string parametersFilename;
    std::string bgFilename;
    std::string inputVideoFilename;
    std::string zonesFilename;
    int inputDevice;

    int startTime;
    int durationTime;

    // todo move into simple tags
    bool simpleTag;
    float simpleTagWidth;
    float simpleTagHeight;
    float simpleTagMinBlobSize;

    Parameters ();
    void loadXML (std::string filename);
    void parseCommandLine (int argc, char** argv);
};


extern Parameters parameters;


#endif
