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

#include "Curves.h"

#include <algorithm>
#include <cmath>

using namespace cv;
using namespace std;

Curves::Curves() : PipelinePlugin()
{
    // enhancement plugin: uses global statistics-free per-pixel LUT, but keep
    // it single threaded so the dialog edits one shared state
    multithreaded = false;

    for (int c = 0; c < NUM_CHANNELS; c++)
	SetIdentity(c);
}

Curves::~Curves()
{
}

void Curves::SetIdentity(int channel)
{
    points[channel].clear();
    points[channel].push_back(Point2f(0.0f, 0.0f));
    points[channel].push_back(Point2f(255.0f, 255.0f));
    lutDirty = true;
}

void Curves::Reset()
{
    lutDirty = true;
    BuildLUT();
}

// monotone cubic (Fritsch-Carlson) interpolation of the control points into a
// 256 entry lookup table, clamped to [0,255], guaranteed non-overshooting
void Curves::BuildCurveLUT (const vector<Point2f>& ptsIn, unsigned char out[256])
{
    // copy + sanitize: sort by x, ensure endpoints at 0 and 255
    vector<Point2f> p = ptsIn;
    if (p.size() < 2)
    {
	for (int x = 0; x < 256; x++) out[x] = (unsigned char)x;
	return;
    }
    sort(p.begin(), p.end(), [](const Point2f& a, const Point2f& b){ return a.x < b.x; });

    int n = (int)p.size();

    // secant slopes
    vector<float> d(n - 1);
    for (int i = 0; i < n - 1; i++)
    {
	float dx = p[i + 1].x - p[i].x;
	d[i] = (dx > 1e-6f) ? (p[i + 1].y - p[i].y) / dx : 0.0f;
    }

    // tangents
    vector<float> m(n);
    m[0] = d[0];
    m[n - 1] = d[n - 2];
    for (int i = 1; i < n - 1; i++)
	m[i] = (d[i - 1] + d[i]) / 2.0f;

    // enforce monotonicity
    for (int i = 0; i < n - 1; i++)
    {
	if (fabs(d[i]) < 1e-6f)
	{
	    m[i] = 0.0f;
	    m[i + 1] = 0.0f;
	}
	else
	{
	    float a = m[i] / d[i];
	    float b = m[i + 1] / d[i];
	    float s = a * a + b * b;
	    if (s > 9.0f)
	    {
		float t = 3.0f / sqrtf(s);
		m[i] = t * a * d[i];
		m[i + 1] = t * b * d[i];
	    }
	}
    }

    // evaluate for every 0..255 input
    int seg = 0;
    for (int x = 0; x < 256; x++)
    {
	float fx = (float)x;

	if (fx <= p.front().x) { out[x] = saturate_cast<uchar>(p.front().y); continue; }
	if (fx >= p.back().x)  { out[x] = saturate_cast<uchar>(p.back().y);  continue; }

	while (seg < n - 2 && fx > p[seg + 1].x) seg++;
	while (seg > 0 && fx < p[seg].x) seg--;

	float h = p[seg + 1].x - p[seg].x;
	float t = (h > 1e-6f) ? (fx - p[seg].x) / h : 0.0f;
	float t2 = t * t;
	float t3 = t2 * t;

	float h00 = 2 * t3 - 3 * t2 + 1;
	float h10 = t3 - 2 * t2 + t;
	float h01 = -2 * t3 + 3 * t2;
	float h11 = t3 - t2;

	float y = h00 * p[seg].y + h10 * h * m[seg]
		+ h01 * p[seg + 1].y + h11 * h * m[seg + 1];

	out[x] = saturate_cast<uchar>(y);
    }
}

void Curves::BuildLUT()
{
    unsigned char master[256], curveR[256], curveG[256], curveB[256];
    BuildCurveLUT(points[MASTER], master);
    BuildCurveLUT(points[RED],    curveR);
    BuildCurveLUT(points[GREEN],  curveG);
    BuildCurveLUT(points[BLUE],   curveB);

    // final per-channel LUT = channelCurve(masterCurve(x))
    lutMat.create(1, 256, CV_8UC3);
    Vec3b* row = lutMat.ptr<Vec3b>(0);
    for (int x = 0; x < 256; x++)
    {
	unsigned char mx = master[x];
	// OpenCV Mat channel order is BGR
	row[x][0] = curveB[mx];
	row[x][1] = curveG[mx];
	row[x][2] = curveR[mx];
    }

    lutDirty = false;
}

void Curves::Apply()
{
    if (lutDirty || lutMat.empty()) BuildLUT();

    if (pipeline->frame.channels() == 3)
    {
	LUT(pipeline->frame, lutMat, pipeline->frame);
    }
    else if (pipeline->frame.channels() == 1)
    {
	// grayscale: apply the master curve alone
	unsigned char master[256];
	BuildCurveLUT(points[MASTER], master);
	Mat lut1(1, 256, CV_8UC1, master);
	LUT(pipeline->frame, lut1, pipeline->frame);
    }
}

// serialize control points as a flat [x0,y0,x1,y1,...] list per channel
static void SavePoints (FileStorage& fs, const string& name, const vector<Point2f>& pts)
{
    fs << name << "[";
    for (auto& p : pts) fs << p.x << p.y;
    fs << "]";
}

static void LoadPoints (const FileNode& fn, vector<Point2f>& pts)
{
    if (fn.empty() || !fn.isSeq()) return;

    // iterate the sequence node element by element (operator>> into a vector
    // would use binary readRawData, which does not match a text sequence)
    vector<float> flat;
    for (FileNodeIterator it = fn.begin(); it != fn.end(); ++it)
	flat.push_back((float)(*it));

    if (flat.size() < 4 || (flat.size() % 2) != 0) return; // keep default
    pts.clear();
    for (size_t i = 0; i + 1 < flat.size(); i += 2)
	pts.push_back(Point2f(flat[i], flat[i + 1]));
}

void Curves::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	if (!fn["EditChannel"].empty()) editChannel = (int)fn["EditChannel"];
	LoadPoints(fn["MasterPoints"], points[MASTER]);
	LoadPoints(fn["RedPoints"],    points[RED]);
	LoadPoints(fn["GreenPoints"],  points[GREEN]);
	LoadPoints(fn["BluePoints"],   points[BLUE]);
	lutDirty = true;
    }
}

void Curves::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "EditChannel" << editChannel;
    SavePoints(fs, "MasterPoints", points[MASTER]);
    SavePoints(fs, "RedPoints",    points[RED]);
    SavePoints(fs, "GreenPoints",  points[GREEN]);
    SavePoints(fs, "BluePoints",   points[BLUE]);
}
