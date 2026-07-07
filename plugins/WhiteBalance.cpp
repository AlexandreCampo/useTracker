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

#include "WhiteBalance.h"

#include "ImageProcessingEngine.h"

#include <vector>

using namespace cv;
using namespace std;

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
    else if (type == GRAYWORLD)
    {
	Ptr<xphoto::GrayworldWB> gw = xphoto::createGrayworldWB();
	gw->setSaturationThreshold(saturationThreshold);
	wb = gw;
    }
    else
    {
	wb.release(); // MANUAL / UNDERWATER do not use xphoto
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

void WhiteBalance::AddPick (Point p)
{
    pendingPick = p;
    hasPick = true;
}

void WhiteBalance::ResetGains()
{
    gainB = gainG = gainR = 1.0f;
}

// compute per-channel gains that turn the given pixel neutral grey
void WhiteBalance::ComputeGainsFromPixel (const Vec3b& bgr)
{
    float b = bgr[0], g = bgr[1], r = bgr[2];
    float target = (b + g + r) / 3.0f;
    if (target < 1.0f) target = 1.0f;

    // guard against near-black channels, clamp gains to a sane range
    gainB = (b > 1.0f) ? target / b : 1.0f;
    gainG = (g > 1.0f) ? target / g : 1.0f;
    gainR = (r > 1.0f) ? target / r : 1.0f;

    gainB = std::min(std::max(gainB, 0.2f), 5.0f);
    gainG = std::min(std::max(gainG, 0.2f), 5.0f);
    gainR = std::min(std::max(gainR, 0.2f), 5.0f);
}

void WhiteBalance::ApplyManual()
{
    // per-channel linear gain via a lookup table
    Mat lut(1, 256, CV_8UC3);
    Vec3b* row = lut.ptr<Vec3b>(0);
    for (int x = 0; x < 256; x++)
    {
	row[x][0] = saturate_cast<uchar>(x * gainB);
	row[x][1] = saturate_cast<uchar>(x * gainG);
	row[x][2] = saturate_cast<uchar>(x * gainR);
    }
    LUT(pipeline->frame, lut, pipeline->frame);
}

void WhiteBalance::ApplyUnderwater()
{
    // Red channel compensation (Ancuti et al.): underwater, red light is
    // absorbed first, so restore it from the better-preserved green channel,
    // then normalise the overall colour with a gray-world pass.
    Mat f32;
    pipeline->frame.convertTo(f32, CV_32F, 1.0 / 255.0);

    vector<Mat> ch;
    split(f32, ch); // B, G, R in [0,1]

    Scalar m = mean(f32);
    float mG = (float)m[1], mR = (float)m[2];

    // compensated red = R + k*(meanG - meanR)*(1 - R)*G
    Mat comp = redCompensation * (mG - mR) * (1.0 - ch[2]).mul(ch[1]);
    ch[2] = ch[2] + comp;

    merge(ch, f32);

    // gray-world normalisation on the compensated image
    Scalar m2 = mean(f32);
    float avg = (float)(m2[0] + m2[1] + m2[2]) / 3.0f;
    split(f32, ch);
    for (int c = 0; c < 3; c++)
    {
	float mc = (float)m2[c];
	if (mc > 1e-4f) ch[c] = ch[c] * (avg / mc);
    }
    merge(ch, f32);

    f32.convertTo(pipeline->frame, CV_8U, 255.0);
}

void WhiteBalance::Apply()
{
    if (pipeline->frame.channels() != 3) return;

    // service a pending "pick white" request: sample the pixel WhiteBalance
    // receives as input (before its own transform) and derive gains from it
    if (hasPick)
    {
	Point p = pendingPick;
	if (p.x >= 0 && p.x < pipeline->frame.cols &&
	    p.y >= 0 && p.y < pipeline->frame.rows)
	{
	    ComputeGainsFromPixel(pipeline->frame.at<Vec3b>(p.y, p.x));
	    type = MANUAL;
	}
	hasPick = false;
    }

    if (type == MANUAL)
    {
	ApplyManual();
    }
    else if (type == UNDERWATER)
    {
	ApplyUnderwater();
    }
    else
    {
	if (!wb) Reset();
	wb->balanceWhite(pipeline->frame, balanced);
	balanced.copyTo(pipeline->frame);
    }
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
	if (!fn["GainB"].empty()) gainB = (float)fn["GainB"];
	if (!fn["GainG"].empty()) gainG = (float)fn["GainG"];
	if (!fn["GainR"].empty()) gainR = (float)fn["GainR"];
	if (!fn["RedCompensation"].empty()) redCompensation = (float)fn["RedCompensation"];

	if (type < GRAYWORLD || type > UNDERWATER) type = GRAYWORLD;
	if (saturationThreshold <= 0.0f || saturationThreshold > 1.0f)
	    saturationThreshold = 0.9f;
    }
}

void WhiteBalance::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Type" << type;
    fs << "SaturationThreshold" << saturationThreshold;
    fs << "GainB" << gainB;
    fs << "GainG" << gainG;
    fs << "GainR" << gainR;
    fs << "RedCompensation" << redCompensation;
}
