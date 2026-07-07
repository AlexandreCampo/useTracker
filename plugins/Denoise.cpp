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

#include "Denoise.h"

using namespace cv;

Denoise::Denoise() : PipelinePlugin()
{
    // spatial filters have a neighbourhood: run on the whole frame to avoid
    // seams at thread slice borders
    multithreaded = false;
}

Denoise::~Denoise()
{
}

void Denoise::Reset()
{
}

void Denoise::Apply()
{
    if (pipeline->frame.channels() != 3) return;

    if (method == NLMEANS)
    {
	fastNlMeansDenoisingColored(pipeline->frame, tmp,
				    nlmStrength, nlmColorStrength, 7, 21);
    }
    else
    {
	// bilateralFilter cannot run in place
	bilateralFilter(pipeline->frame, tmp, diameter, sigmaColor, sigmaSpace);
    }

    tmp.copyTo(pipeline->frame);
}

void Denoise::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	if (!fn["Method"].empty()) method = (int)fn["Method"];
	if (!fn["Diameter"].empty()) diameter = (int)fn["Diameter"];
	if (!fn["SigmaColor"].empty()) sigmaColor = (float)fn["SigmaColor"];
	if (!fn["SigmaSpace"].empty()) sigmaSpace = (float)fn["SigmaSpace"];
	if (!fn["NlmStrength"].empty()) nlmStrength = (float)fn["NlmStrength"];
	if (!fn["NlmColorStrength"].empty()) nlmColorStrength = (float)fn["NlmColorStrength"];

	if (method != BILATERAL && method != NLMEANS) method = BILATERAL;
    }
}

void Denoise::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Method" << method;
    fs << "Diameter" << diameter;
    fs << "SigmaColor" << sigmaColor;
    fs << "SigmaSpace" << sigmaSpace;
    fs << "NlmStrength" << nlmStrength;
    fs << "NlmColorStrength" << nlmColorStrength;
}
