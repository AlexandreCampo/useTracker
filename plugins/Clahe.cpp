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

#include "Clahe.h"

#include "ImageProcessingEngine.h"

using namespace cv;

Clahe::Clahe() : PipelinePlugin()
{
    // single threaded: CLAHE tiles would show seams at thread slice borders
    multithreaded = false;
}

Clahe::~Clahe()
{
}

void Clahe::Reset()
{
    clahe = createCLAHE(clipLimit, Size(tileSize, tileSize));
}

void Clahe::SetClipLimit(float c)
{
    if (c < 0.1f) c = 0.1f;
    clipLimit = c;
    if (clahe) clahe->setClipLimit(c);
}

void Clahe::SetTileSize(int s)
{
    if (s < 1) s = 1;
    tileSize = s;
    if (clahe) clahe->setTilesGridSize(Size(s, s));
}

void Clahe::Apply()
{
    if (pipeline->frame.channels() == 3)
    {
	// equalize the lightness channel only, hue is preserved
	cvtColor(pipeline->frame, lab, COLOR_BGR2Lab);
	split(lab, channels);
	clahe->apply(channels[0], channels[0]);
	merge(channels, lab);
	cvtColor(lab, pipeline->frame, COLOR_Lab2BGR);
    }
    else
    {
	clahe->apply(pipeline->frame, pipeline->frame);
    }
}

void Clahe::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	if (!fn["ClipLimit"].empty())
	    clipLimit = (float)fn["ClipLimit"];
	if (!fn["TileSize"].empty())
	    tileSize = (int)fn["TileSize"];

	if (clipLimit < 0.1f) clipLimit = 3.0f;
	if (tileSize < 1) tileSize = 8;
    }
}

void Clahe::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "ClipLimit" << clipLimit;
    fs << "TileSize" << tileSize;
}
