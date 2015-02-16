
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

    bool nogui = false;

    std::string parametersFilename;
    std::string bgFilename;
    std::string inputFilename;
    std::string zonesFilename;
    int usbDevice = -1;
    int avtDevice = -1;
    
    int startTime = -1;
    int durationTime = -1;

    void loadXML (std::string filename);
    void parseCommandLine (int argc, char** argv);
};


extern Parameters parameters;


#endif
