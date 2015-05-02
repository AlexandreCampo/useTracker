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

#ifndef SIMPLE_TAGS_H
#define SIMPLE_TAGS_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>


class SimpleTags : public PipelinePlugin
{
public:

    cv::Mat pattern;
    std::vector<int> p;
    int pwidth = 100;
    int pheight = 20;

    std::string outputFilename;
    std::fstream outputStream;

    SimpleTags();
    ~SimpleTags();

    void Reset();
    void Apply();
    void OutputHud (cv::Mat& hud);
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void OutputStep();
    void OpenOutput();
    void CloseOutput();

    void SetTagDimensions(int width, int height);
};


#endif
