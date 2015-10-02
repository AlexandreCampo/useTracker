/*----------------------------------------------------------------------------*/
/*    Copyright (C) 2015 Alexandre Campo                                      */
/*                                                                            */
/*    This file is part of USE Tracker.                                       */
/*                                                                            */
/*    USE Tracker is free software: you can redistribute it and/or modify     */
/*    it under the terms of the GNU General Public License as published by    */
/*    the Free Software Foundation, either version 3 of the License, or       */
/*    (at your option) any later version.                                     */
/*                                                                            */
/*    USE Tracker is distributed in the hope that it will be useful,          */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/*    GNU General Public License for more details.                            */
/*                                                                            */
/*    You should have received a copy of the GNU General Public License       */
/*    along with USE Tracker.  If not, see <http://www.gnu.org/licenses/>.    */
/*----------------------------------------------------------------------------*/

#include "Parameters.h"

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace po = boost::program_options;

// yes, this is a global variable...
Parameters parameters;

void Parameters::parseCommandLine (int argc, char** argv)
{
    // Parse command line with lib boost
    po::options_description desc("\nAllowed options");
    desc.add_options()
	("help,h", "produce help message")
	("nogui,n", "command line only, disable graphical interface")
	("parameters,p", po::value<string>(), "configuration file with parameters in xml format")
	("inputfile,i", po::value<vector<string>>()->multitoken(), "use specified video file(s)")
	("usb,u", po::value<vector<int>>()->multitoken(), "use specified USB camera device(s)")
	("avt,a", po::value<int>(), "use specified AVT camera device")
	("mask,m", po::value<string>(), "mask image for zones of interest filename")
	("start", po::value<float>(), "start time of tracking")
	("length,l", po::value<float>(), "length (duration) of tracking")
	("stitching", po::value<string>(), "stitching configuration file in xml format ")
	("calibration", po::value<string>(), "calibration configuration file in xml format ")
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

    if (vm.count("usb"))
    {
	usbDevices = vm["usb"].as<vector<int>>();

	if (usbDevices.size() == 1)
	{
	    usbDevice = usbDevices[0];
	    std::cout << "Using USB camera, device=" << usbDevice << " as data source" << std::endl;
	}
	else
	{
	    multiUSBCapture = true;
	    std::cout << "Using multiple USB cameras, devices=";
	    std::cout << usbDevices[0];
	    for (unsigned int i = 1; i < usbDevices.size(); i++)
	    {	    
		std::cout << ", " << usbDevices[i];
	    }
	    std::cout << " as data source" << std::endl;
	}
    }

    if (vm.count("avt"))
    {
	avtDevice = vm["avt"].as<int>();
	std::cout << "Using AVT camera, device=" << avtDevice << " as data source" << std::endl;
    }

    if (vm.count("inputfile"))
    {       
	inputFilenames = vm["inputfile"].as<vector<string>>();

	if (inputFilenames.size() == 1)
	{
	    inputFilename = inputFilenames[0];
	    std::cout << "Using file " << inputFilename << " as data source" << std::endl;
	}
	else
	{
	    multiVideoCapture = true;
	    std::cout << "Using multiple files : ";
	    std::cout << inputFilenames[0];
	    for (unsigned int i = 1; i < inputFilenames.size(); i++)
	    {	    
		std::cout << ", " << inputFilenames[i];
	    }
	    std::cout << " as data source" << std::endl;
	}
    }

    if (vm.count("mask"))
    {
	zonesFilename = vm["mask"].as<string>();
	std::cout << "Using mask image for zones of interest " << zonesFilename << " as mask" << std::endl;
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

    if (vm.count("stitching"))
    {
	stitchingFilename = vm["stitching"].as<string>();
	std::cout << "Using stitching configuration from file " << stitchingFilename << std::endl;
    }

    if (vm.count("calibration"))
    {
	calibrationFilename = vm["calibration"].as<string>();
	std::cout << "Using calibration configuration from file " << calibrationFilename << std::endl;
    }

    std::cout << "Check possible command line switches with --help !" << std::endl << std::endl;

}

