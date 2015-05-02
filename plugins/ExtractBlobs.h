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
    unsigned int maxSize = 0;

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

