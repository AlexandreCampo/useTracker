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

#ifndef WHITE_BALANCE_H
#define WHITE_BALANCE_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/xphoto/white_balance.hpp>

// Automatic white balance. Image enhancement plugin: modifies the capture
// frame in place, removing color casts (e.g. the blue-green cast of
// underwater footage) so that objects pop for the human eye and for the
// downstream detection plugins. The engine restores the pristine source
// frame on static re-steps and RecordVideo records the original.
class WhiteBalance : public PipelinePlugin
{
public:

    enum Type {GRAYWORLD = 0, SIMPLE = 1};

    int type = GRAYWORLD;
    float saturationThreshold = 0.9f; // grayworld only

    cv::Ptr<cv::xphoto::WhiteBalancer> wb;
    cv::Mat balanced;

    WhiteBalance();
    ~WhiteBalance();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void SetType(int t);
    void SetSaturationThreshold(float s);
};

#endif
