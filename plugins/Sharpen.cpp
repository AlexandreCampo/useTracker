/*----------------------------------------------------------------------------*/
/*    Copyright (C) 2015 Alexandre Campo                                      */
/*                                                                            */
/*    This file is part of USE Tracker.                                       */
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

#include "Sharpen.h"

using namespace cv;

Sharpen::Sharpen() : PipelinePlugin()
{
    // uses a Gaussian neighbourhood: run on the whole frame (no slice seams)
    multithreaded = false;
}

Sharpen::~Sharpen()
{
}

void Sharpen::Reset()
{
}

void Sharpen::Apply()
{
    float r = radius;
    if (r < 0.1f) r = 0.1f;

    // unsharp mask: sharp = (1 + amount) * src - amount * blur
    GaussianBlur(pipeline->frame, blurred, Size(0, 0), r);
    addWeighted(pipeline->frame, 1.0 + amount, blurred, -(double)amount, 0.0,
		pipeline->frame);
}

void Sharpen::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	if (!fn["Amount"].empty()) amount = (float)fn["Amount"];
	if (!fn["Radius"].empty()) radius = (float)fn["Radius"];
    }
}

void Sharpen::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Amount" << amount;
    fs << "Radius" << radius;
}
