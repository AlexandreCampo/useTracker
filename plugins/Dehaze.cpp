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

#include "Dehaze.h"

#include <vector>

using namespace cv;
using namespace std;

Dehaze::Dehaze() : PipelinePlugin()
{
    // atmospheric light is a global estimate: run on the whole frame
    multithreaded = false;
}

Dehaze::~Dehaze()
{
}

void Dehaze::Reset()
{
}

void Dehaze::Apply()
{
    if (pipeline->frame.channels() != 3) return;

    int patch = std::max(3, patchSize);
    float w = std::min(std::max(omega, 0.0f), 1.0f);
    float tmin = std::min(std::max(t0, 0.01f), 1.0f);

    Mat f;
    pipeline->frame.convertTo(f, CV_32F, 1.0 / 255.0);

    vector<Mat> ch;
    split(f, ch); // B, G, R in [0,1]

    Mat kernel = getStructuringElement(MORPH_RECT, Size(patch, patch));

    // dark channel = min over channels, then min-filter (erosion)
    Mat minc;
    cv::min(ch[0], ch[1], minc);
    cv::min(minc, ch[2], minc);
    Mat dark;
    erode(minc, dark, kernel);

    // atmospheric light A: average colour of the haziest (brightest dark
    // channel) pixels
    double mn, mx;
    minMaxLoc(dark, &mn, &mx);
    Mat brightMask = dark >= (mx * 0.9);
    Scalar A = mean(f, brightMask);
    float Ab = std::max((float)A[0], 1e-3f);
    float Ag = std::max((float)A[1], 1e-3f);
    float Ar = std::max((float)A[2], 1e-3f);

    // transmission t = 1 - omega * darkChannel(I / A)
    Mat nb = ch[0] / Ab, ng = ch[1] / Ag, nr = ch[2] / Ar;
    Mat na;
    cv::min(nb, ng, na);
    cv::min(na, nr, na);
    Mat darkA;
    erode(na, darkA, kernel);
    Mat t = 1.0 - w * darkA;

    // light refinement + clamp
    GaussianBlur(t, t, Size(0, 0), patch);
    cv::max(t, (double)tmin, t);

    // recover J = (I - A) / t + A
    ch[0] = (ch[0] - Ab) / t + Ab;
    ch[1] = (ch[1] - Ag) / t + Ag;
    ch[2] = (ch[2] - Ar) / t + Ar;
    merge(ch, f);

    f.convertTo(pipeline->frame, CV_8U, 255.0);
}

void Dehaze::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	if (!fn["Omega"].empty()) omega = (float)fn["Omega"];
	if (!fn["PatchSize"].empty()) patchSize = (int)fn["PatchSize"];
	if (!fn["T0"].empty()) t0 = (float)fn["T0"];
    }
}

void Dehaze::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Omega" << omega;
    fs << "PatchSize" << patchSize;
    fs << "T0" << t0;
}
