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
#include <CLI/CLI.hpp>

using namespace std;
using namespace cv;

// yes, this is a global variable...
Parameters parameters;

void Parameters::parseCommandLine (int argc, char** argv)
{
    CLI::App app{"USE Tracker - real-time image processing and blob tracking"};

    app.add_flag("-n,--nogui", nogui, "Command line only, disable graphical interface");
    app.add_option("-p,--parameters", parametersFilename, "Configuration file with parameters in xml format");
    app.add_option("-i,--inputfile", inputFilenames, "Use specified video file(s)")->expected(-1);
    app.add_option("-u,--usb", usbDevices, "Use specified USB camera device(s)")->expected(-1);
    app.add_option("-m,--mask", zonesFilename, "Mask image for zones of interest filename");
    float startTimeF = -1;
    app.add_option("--start", startTimeF, "Start time of tracking");
    float durationTimeF = -1;
    app.add_option("-l,--length", durationTimeF, "Length (duration) of tracking");
    app.add_option("--stitching", stitchingFilename, "Stitching configuration file in xml format");
    app.add_option("--calibration", calibrationFilename, "Calibration configuration file in xml format");
    app.add_option("-b,--background", bgFilename, "Specify the background image for motion detection");
    app.add_option("--test", testScript, "GUI test script: drive the interface and take screenshots");
    app.add_option("--scale", inputScale, "Downscale input frames by this factor (0.1-1.0) for faster processing");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        exit(app.exit(e));
    }

    if (startTimeF >= 0) startTime = startTimeF;
    if (durationTimeF >= 0) durationTime = durationTimeF;

    if (!parametersFilename.empty())
    {
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

    if (nogui)
    {
	std::cout << "Graphical interface disabled " << std::endl;
    }

    if (!usbDevices.empty())
    {
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

    if (!inputFilenames.empty())
    {
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

    if (!zonesFilename.empty())
	std::cout << "Using mask image for zones of interest " << zonesFilename << " as mask" << std::endl;

    if (startTime >= 0)
	std::cout << "Start tracking at time t=" << startTime << " seconds" << std::endl;

    if (durationTime >= 0)
	std::cout << "Tracking during " << durationTime << " seconds" << std::endl;

    if (!bgFilename.empty())
	std::cout << "Using image " << bgFilename << " as background" << std::endl;

    if (!stitchingFilename.empty())
	std::cout << "Using stitching configuration from file " << stitchingFilename << std::endl;

    if (!calibrationFilename.empty())
	std::cout << "Using calibration configuration from file " << calibrationFilename << std::endl;

    std::cout << "Check possible command line switches with --help !" << std::endl << std::endl;
}

void Parameters::loadXML (std::string filename)
{
    if (file.isOpened()) file.release();

    try
    {
	file.open(filename, FileStorage::READ);
	if (file.isOpened())
	{
	    rootNode = file["Configuration"];
	    parametersFilename = filename;
	}
	else
	{
	    std::cerr << "Error: Could not load parameters file " << filename << std::endl;
	}
    }
    catch (cv::Exception& e)
    {
	std::cout << "exception caught: " << e.what() << std::endl;
    }
}
