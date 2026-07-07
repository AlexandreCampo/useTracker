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
/*----------------------------------------------------------------------------*/

#include "ZonesOfInterest.h"

#include "ImageProcessingEngine.h"

#include <opencv2/imgcodecs.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

using namespace cv;
using namespace std;

ZonesOfInterest::ZonesOfInterest() : PipelinePlugin()
{
    // single threaded: defining the whole-frame zone map is a global operation
    multithreaded = false;
}

void ZonesOfInterest::Reset()
{
    if (!zonesImageFilename.empty() && zonesImage.empty())
	LoadImage(zonesImageFilename);
    Rasterize();
}

void ZonesOfInterest::LoadImage(const std::string& filename)
{
    zonesImageFilename = filename;
    zonesImage = imread(filename, IMREAD_GRAYSCALE);
    if (zonesImage.empty())
	cerr << "Could not load zones image " << filename << endl;
}

void ZonesOfInterest::Rasterize()
{
    if (!pipeline || !pipeline->parent) return;
    Mat& zoneMap = pipeline->parent->zoneMap;
    if (zoneMap.empty()) return;

    // base: the zone image if it matches the frame, otherwise a blank map.
    // With no polygons and no image the whole frame is visible (region 1) so
    // an empty / freshly added plugin does not blank everything out.
    if (!zonesImage.empty() && zonesImage.size() == zoneMap.size())
	zonesImage.copyTo(zoneMap);
    else
	zoneMap.setTo((polygons.empty() && zonesImage.empty()) ? 1 : 0);

    // draw polygons on top (region 0 is left as background = ignored)
    for (auto& poly : polygons)
    {
	if (poly.points.size() < 3 || poly.region <= 0) continue;
	vector<vector<Point>> pts { poly.points };
	fillPoly(zoneMap, pts, Scalar(poly.region), LINE_8);
    }
}

void ZonesOfInterest::Apply()
{
    // restrict the running mask to the visible zones (region > 0)
    Mat zoneBin;
    threshold(pipeline->zoneMap, zoneBin, 0, 255, THRESH_BINARY);
    pipeline->marked &= zoneBin;
}

// ---- polygon text file ------------------------------------------------------

bool ZonesOfInterest::LoadRois(const std::string& filename)
{
    ifstream f(filename);
    if (!f.is_open())
    {
	cerr << "Could not open ROI file " << filename << endl;
	return false;
    }

    vector<Polygon> loaded;
    Polygon* current = nullptr;
    string line;
    while (getline(f, line))
    {
	size_t hash = line.find('#');
	if (hash != string::npos) line = line.substr(0, hash);
	istringstream ss(line);

	string tok;
	if (!(ss >> tok)) continue;

	if (tok == "polygon")
	{
	    int region = 1;
	    ss >> region;
	    loaded.push_back(Polygon());
	    loaded.back().region = region;
	    current = &loaded.back();
	}
	else
	{
	    try
	    {
		int x = stoi(tok), y;
		if (!(ss >> y)) continue;
		if (!current) { loaded.push_back(Polygon()); current = &loaded.back(); }
		current->points.push_back(Point(x, y));
	    }
	    catch (...) { continue; }
	}
    }

    polygons = loaded;
    roiFilename = filename;
    return true;
}

void ZonesOfInterest::SaveRois(const std::string& filename)
{
    ofstream f(filename);
    if (!f.is_open())
    {
	cerr << "Could not write ROI file " << filename << endl;
	return;
    }
    f << "# useTracker regions of interest\n";
    f << "# each polygon: 'polygon <region>' then one 'x y' vertex per line\n";
    for (auto& poly : polygons)
    {
	f << "polygon " << poly.region << "\n";
	for (auto& p : poly.points) f << p.x << " " << p.y << "\n";
	f << "\n";
    }
    roiFilename = filename;
}

// ---- settings XML -----------------------------------------------------------
// polygons are stored inline as a flat sequence:
//   region, npoints, x0,y0, x1,y1, ...  (repeated per polygon)

void ZonesOfInterest::LoadXML (FileNode& fn)
{
    if (fn.empty()) return;

    active = (int)fn["Active"];
    roiFilename = (string)fn["RoiFilename"];
    zonesImageFilename = (string)fn["ZonesImageFilename"];

    polygons.clear();
    FileNode pn = fn["Polygons"];
    if (pn.isSeq())
    {
	vector<float> flat;
	for (FileNodeIterator it = pn.begin(); it != pn.end(); ++it)
	    flat.push_back((float)(*it));

	size_t i = 0;
	while (i + 1 < flat.size())
	{
	    Polygon poly;
	    poly.region = (int)flat[i++];
	    int n = (int)flat[i++];
	    for (int k = 0; k < n && i + 1 < flat.size(); k++)
	    {
		int x = (int)flat[i++];
		int y = (int)flat[i++];
		poly.points.push_back(Point(x, y));
	    }
	    polygons.push_back(poly);
	}
    }

    // an external ROI file, if given, takes precedence
    if (!roiFilename.empty())
	LoadRois(roiFilename);
    if (!zonesImageFilename.empty())
	LoadImage(zonesImageFilename);
}

void ZonesOfInterest::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "RoiFilename" << roiFilename;
    fs << "ZonesImageFilename" << zonesImageFilename;

    fs << "Polygons" << "[";
    for (auto& poly : polygons)
    {
	fs << poly.region << (int)poly.points.size();
	for (auto& p : poly.points) fs << p.x << p.y;
    }
    fs << "]";
}
