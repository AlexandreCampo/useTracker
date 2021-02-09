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

#include "TakeSnapshots.h"

#include "ImageProcessingEngine.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace cv;
using namespace std;

TakeSnapshots::~TakeSnapshots()
{
    CloseOutput();
}

void TakeSnapshots::Apply()
{
}

void TakeSnapshots::OutputStep()
{
    if (openOutput)
    {
	// figure the filename to write snapshot
	path fullpath (outputFilename);
	path parent = fullpath.parent_path();
	path stem = fullpath.stem();

	path extension;
	if (fullpath.has_extension())
	    extension = fullpath.extension();
	else
	    extension = path(".png");

	// ok now append frame number
	std::ostringstream tmp;
	tmp << std::setw(9) << std::setfill('0') << pipeline->parent->frameNumber;
	string fname ((parent / stem).string() + "_" + tmp.str() + extension.string());

	imwrite(fname, min(pipeline->zoneMap, pipeline->marked));
    }
}

void TakeSnapshots::CloseOutput()
{
    openOutput = false;
}

void TakeSnapshots::OpenOutput()
{
    openOutput = true;
}

void TakeSnapshots::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	output = (int)fn["Output"];
	outputFilename = (string)fn["OutputFilename"];
   }
}

void TakeSnapshots::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Output" << output;
    fs << "OutputFilename" << outputFilename;
}
