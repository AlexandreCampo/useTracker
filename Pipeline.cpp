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

#include "Pipeline.h"

#include "ImageProcessingEngine.h"

Pipeline::Pipeline (ImageProcessingEngine* ipEngine, cv::Rect roi)
{
    this->parent = ipEngine;
    this->roi = roi;

    width = roi.width;
    height = roi.height;

    frame = ipEngine->capture->frame(roi);
    background = ipEngine->background(roi);
    zoneMap = ipEngine->zoneMap(roi);
    marked = ipEngine->marked(roi);
    labels = ipEngine->labels(roi);
    pipelineSnapshotMarked = ipEngine->pipelineSnapshotMarked(roi);
}

Pipeline::~Pipeline()
{
    for (auto p : plugins)
    {
	if (p) delete (p);
    }
}

void Pipeline::Reset (cv::Rect roi)
{
    // adjust roi and locate new data source, but don't delete plugins, settings etc...
    this->roi = roi;

    width = roi.width;
    height = roi.height;

    frame = parent->capture->frame(roi);
    background = parent->background(roi);
    zoneMap = parent->zoneMap(roi);
    marked = parent->marked(roi);
    labels = parent->labels(roi);
    pipelineSnapshotMarked = parent->pipelineSnapshotMarked(roi);

    // reset plugins to take into account new data
    for (auto p : plugins)
    {
	if (p) p->Reset();
    }
}
