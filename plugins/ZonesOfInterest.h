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

#ifndef ZONES_OF_INTEREST_H
#define ZONES_OF_INTEREST_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>

#include <vector>
#include <string>

// Regions of interest. Defines which parts of the frame are processed. Zones
// are described as polygons drawn directly on the video (each assigned a
// region number: 0 and un-painted areas are ignored, 1+ are kept), and/or
// loaded from a grayscale zone-mask image. The plugin rasterizes them into
// the engine's shared zoneMap, which downstream plugins read (both as an
// overall mask and via their "Restrict to Zone" option), and it also masks
// the running result to the visible zones.
class ZonesOfInterest : public PipelinePlugin
{
public:

    struct Polygon
    {
	std::vector<cv::Point> points;
	int region = 1;
    };

    std::vector<Polygon> polygons;
    std::string roiFilename;          // polygon text file
    std::string zonesImageFilename;   // optional grayscale zone-mask image
    cv::Mat zonesImage;               // loaded image (region numbers)

    ZonesOfInterest();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    // rasterize polygons (over the optional zone image) into the engine zoneMap
    void Rasterize();

    void LoadImage (const std::string& filename);
    bool LoadRois (const std::string& filename);
    void SaveRois (const std::string& filename);
};

#endif
