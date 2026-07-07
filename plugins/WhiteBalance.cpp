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

#include "WhiteBalance.h"

#include "ImageProcessingEngine.h"

using namespace cv;

WhiteBalance::WhiteBalance() : PipelinePlugin()
{
    // single threaded: white balance uses global image statistics
    multithreaded = false;
}

WhiteBalance::~WhiteBalance()
{
}

void WhiteBalance::Reset()
{
    if (type == SIMPLE)
    {
	wb = xphoto::createSimpleWB();
    }
    else
    {
	Ptr<xphoto::GrayworldWB> gw = xphoto::createGrayworldWB();
	gw->setSaturationThreshold(saturationThreshold);
	wb = gw;
    }
}

void WhiteBalance::SetType(int t)
{
    if (t == type) return;

    type = t;
    Reset();
}

void WhiteBalance::SetSaturationThreshold(float s)
{
    saturationThreshold = s;

    Ptr<xphoto::GrayworldWB> gw = wb.dynamicCast<xphoto::GrayworldWB>();
    if (gw) gw->setSaturationThreshold(s);
}

void WhiteBalance::Apply()
{
    if (pipeline->frame.channels() != 3) return;

    wb->balanceWhite(pipeline->frame, balanced);
    balanced.copyTo(pipeline->frame);
}

void WhiteBalance::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	if (!fn["Type"].empty())
	    type = (int)fn["Type"];
	if (!fn["SaturationThreshold"].empty())
	    saturationThreshold = (float)fn["SaturationThreshold"];

	if (type != GRAYWORLD && type != SIMPLE) type = GRAYWORLD;
	if (saturationThreshold <= 0.0f || saturationThreshold > 1.0f)
	    saturationThreshold = 0.9f;
    }
}

void WhiteBalance::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Type" << type;
    fs << "SaturationThreshold" << saturationThreshold;
}
