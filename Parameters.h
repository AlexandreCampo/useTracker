
#ifndef PARAMETERS_H
#define PARAMETERS_H

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
    std::vector<int> usbDevices;
    std::vector<std::string> inputFilenames;

    std::string stitchingFilename;
    std::string calibrationFilename;

    // GUI test/debug: a script of input commands to drive the interface
    std::string testScript;

    // downscale factor applied to input frames (1.0 = full resolution)
    float inputScale = 1.0f;

    double startTime = -1;
    double durationTime = -1;

    bool multiUSBCapture = false;
    bool multiVideoCapture = false;

    void loadXML (std::string filename);
    void parseCommandLine (int argc, char** argv);
};


extern Parameters parameters;


#endif
