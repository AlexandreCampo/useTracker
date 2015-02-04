
#include "Parameters.h"

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace po = boost::program_options;

// yes, this is a global variable...
Parameters parameters;

Parameters::Parameters ()
{
    // default values
    parametersFilename = string();
    inputVideoFilename = string();
    inputDevice = -1;
    bgFilename = string();
    zonesFilename = string();

    startTime = -1;
    durationTime = -1;

    nogui = false;
}


void Parameters::parseCommandLine (int argc, char** argv)
{
    // Parse command line with lib boost
    po::options_description desc("\nAllowed options");
    desc.add_options()
	("help,h", "produce help message")
	("nogui,n", "command line only, disable graphical interface")
	("parameters,p", po::value<string>(), "configuration file with parameters in xml format")
	("file,f", po::value<string>(), "use specified video file")
	("device,d", po::value<int>(), "use specified USB camera device")
	("avt,a", "use first AVT camera")
	("mask,m", po::value<string>(), "mask image for zones of interest filename")
	("start,s", po::value<float>(), "start time of tracking")
	("length,l", po::value<float>(), "length (duration) of tracking")
	("background,b", po::value<string>(), "specify the background image for motion detection")
	;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
	std::cout << desc << "\n";
	exit (1);
    }

    if (vm.count("parameters"))
    {
	parametersFilename = vm["parameters"].as<string>();
	std::cout << "Reading parameters from file " << parametersFilename << std::endl;

	try
	{
	    file.open(parametersFilename, FileStorage::READ);
	    if (file.isOpened())
	    {
		rootNode = file["Configuration"];
	    }
	    else
	    {
		std::cerr << "Error: Could not load parameters file " << parametersFilename << std::endl;
	    }
	}
	catch( cv::Exception& e )
	{
	    const char* err_msg = e.what();
	    std::cout << "exception caught: " << err_msg << std::endl;
	}
    }

    if (vm.count("nogui"))
    {
	std::cout << "Graphical interface disabled " << std::endl;
	nogui = true;
    }

    if (vm.count("device"))
    {
	inputDevice = vm["device"].as<int>();
	std::cout << "Using USB camera, device=" << inputDevice << " as data source" << std::endl;
    }

    if (vm.count("file"))
    {
	inputVideoFilename = vm["file"].as<string>();
	std::cout << "Using video file " << inputVideoFilename << " as data source" << std::endl;
    }

    if (vm.count("mask"))
    {
	zonesFilename = vm["mask"].as<string>();
	std::cout << "Using mask image for zones of interest" << zonesFilename << " as mask" << std::endl;
    }

    if (vm.count("start"))
    {
	startTime = vm["start"].as<float>();
	std::cout << "Start tracking at time t=" << startTime << " seconds" << std::endl;
    }

    if (vm.count("length"))
    {
	durationTime = vm["length"].as<float>();
	std::cout << "Tracking during " << durationTime << " seconds" << std::endl;
    }

    if (vm.count("background"))
    {
	bgFilename = vm["background"].as<string>();
	std::cout << "Using image " << bgFilename << " as background" << std::endl;
    }

    std::cout << "Check possible command line switches with --help !" << std::endl << std::endl;

}

