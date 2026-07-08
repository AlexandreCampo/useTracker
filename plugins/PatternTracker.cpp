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

#include "PatternTracker.h"

#include "ImageProcessingEngine.h"
#include "Blob.h"

#include <iostream>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace std;

PatternTracker::PatternTracker() : PipelinePlugin()
{
    // appearance matching uses the whole frame and keeps global target state,
    // run it once on the full frame rather than per pipeline slice
    multithreaded = false;
}

PatternTracker::~PatternTracker()
{
    CloseOutput();
}

void PatternTracker::Reset()
{
    detectionMask = Mat::zeros(pipeline->height, pipeline->width, CV_8U);
    targets.clear();
    pendingSeeds.clear();
    nextId = 0;
    backendChanged = false;
    ccCount = 0;
    ccLabels.release();
}

void PatternTracker::SetBackend(int b)
{
    if (b == backend) return;
    backend = b;
    // existing targets must rebuild their backend state from the current frame
    backendChanged = true;
}

void PatternTracker::AddSeed (Point p)
{
    pendingSeeds.push_back(p);
}

void PatternTracker::ClearTargets()
{
    targets.clear();
}

// build a valid box of the given side centered on p, clipped to the frame
static Rect BoxAround (Point2f p, int side, const Mat& frame)
{
    int s = max(8, side);
    int x = (int)round(p.x) - s / 2;
    int y = (int)round(p.y) - s / 2;
    Rect box(x, y, s, s);
    return box & Rect(0, 0, frame.cols, frame.rows);
}

bool PatternTracker::NearExistingTarget (Point2f p, float radius)
{
    for (auto& t : targets)
    {
	if (!t.active) continue;
	float dx = t.pos.x - p.x;
	float dy = t.pos.y - p.y;
	if (dx * dx + dy * dy <= radius * radius) return true;
    }
    return false;
}

// label the incoming foreground mask into connected components so target boxes
// can be taken from the real blob pixels rather than a fixed square
void PatternTracker::ComputeComponents (const Mat& mask)
{
    if (mask.empty())
    {
	ccCount = 0;
	ccLabels.release();
	return;
    }
    Mat bin;
    if (mask.type() != CV_8U) mask.convertTo(bin, CV_8U);
    else bin = mask;
    ccCount = connectedComponentsWithStats(bin, ccLabels, ccStats, ccCentroids, 8, CV_32S);
}

// find the mask blob under p; if p is on background, search outward in rings up
// to searchRadius for the nearest foreground pixel. Returns its bounding box.
bool PatternTracker::ComponentBoxNear (Point p, int searchRadius,
				       Rect& outBox, Point2f& outCentroid)
{
    if (ccCount <= 1 || ccLabels.empty()) return false;
    const int W = ccLabels.cols, H = ccLabels.rows;
    auto labelAt = [&](int x, int y) -> int
    {
	if (x < 0 || y < 0 || x >= W || y >= H) return 0;
	return ccLabels.at<int>(y, x);
    };

    int lab = labelAt(p.x, p.y);
    for (int r = 1; lab <= 0 && r <= searchRadius; r++)
    {
	// scan the square ring at radius r
	for (int dx = -r; dx <= r && lab <= 0; dx++)
	{
	    int l1 = labelAt(p.x + dx, p.y - r);
	    if (l1 > 0) { lab = l1; break; }
	    int l2 = labelAt(p.x + dx, p.y + r);
	    if (l2 > 0) { lab = l2; break; }
	}
	for (int dy = -r + 1; dy <= r - 1 && lab <= 0; dy++)
	{
	    int l1 = labelAt(p.x - r, p.y + dy);
	    if (l1 > 0) { lab = l1; break; }
	    int l2 = labelAt(p.x + r, p.y + dy);
	    if (l2 > 0) { lab = l2; break; }
	}
    }
    if (lab <= 0) return false;

    outBox = Rect(ccStats.at<int>(lab, CC_STAT_LEFT),
		  ccStats.at<int>(lab, CC_STAT_TOP),
		  ccStats.at<int>(lab, CC_STAT_WIDTH),
		  ccStats.at<int>(lab, CC_STAT_HEIGHT));
    outCentroid = Point2f((float)ccCentroids.at<double>(lab, 0),
			  (float)ccCentroids.at<double>(lab, 1));
    return true;
}

// grow/shrink the reported box towards the underlying blob, rate-limited so a
// flickering blob does not make the box jump. When no blob is found this frame
// (occlusion / flicker) the last size is kept and the box just follows pos.
void PatternTracker::RefitBoxToMask (Target& t)
{
    Rect blobBox;
    Point2f blobCentroid;
    if (ComponentBoxNear(Point((int)t.pos.x, (int)t.pos.y), maxDistance,
			 blobBox, blobCentroid))
    {
	t.boxSizeF.width  += sizeAdaptRate * (blobBox.width  - t.boxSizeF.width);
	t.boxSizeF.height += sizeAdaptRate * (blobBox.height - t.boxSizeF.height);
    }

    float w = max(8.0f, t.boxSizeF.width);
    float h = max(8.0f, t.boxSizeF.height);
    Rect b((int)round(t.pos.x - w / 2.0f), (int)round(t.pos.y - h / 2.0f),
	   (int)round(w), (int)round(h));
    t.box = b & Rect(0, 0, pipeline->width, pipeline->height);
}

void PatternTracker::SeedTarget (const Rect& box, const Mat& frame)
{
    if ((int)targets.size() >= maxTargets) return;
    if (box.width < 8 || box.height < 8) return;

    Target t;
    t.id = nextId++;
    t.active = true;
    t.box = box;
    t.boxSizeF = Size2f((float)box.width, (float)box.height);
    t.pos = Point2f(box.x + box.width / 2.0f, box.y + box.height / 2.0f);
    t.velocity = Point2f(0, 0);
    t.score = 1.0f;
    t.lostFrames = 0;

    if (backend == CSRT)
    {
	try
	{
	    t.tracker = TrackerCSRT::create();
	    t.tracker->init(frame, box);
	}
	catch (const cv::Exception& e)
	{
	    cerr << "PatternTracker: CSRT init failed: " << e.what() << endl;
	    return;
	}
    }
    else
    {
	frame(box).copyTo(t.templ);
    }

    targets.push_back(t);
}

// TEMPLATE backend: search a local window for the peak cross-correlation
bool PatternTracker::TrackTemplate (Target& t, const Mat& frame)
{
    if (t.templ.empty()) return false;

    int tw = t.templ.cols;
    int th = t.templ.rows;

    // predicted center of the target this frame
    Point2f predicted = t.pos;
    if (usePrediction) predicted += t.velocity;

    // search region: template size expanded by maxDistance in every direction
    Rect search((int)round(predicted.x) - tw / 2 - maxDistance,
		(int)round(predicted.y) - th / 2 - maxDistance,
		tw + 2 * maxDistance,
		th + 2 * maxDistance);
    search &= Rect(0, 0, frame.cols, frame.rows);

    // the search region must be able to contain the template
    if (search.width < tw || search.height < th) return false;

    Mat result;
    matchTemplate(frame(search), t.templ, result, TM_CCOEFF_NORMED);

    double maxVal;
    Point maxLoc;
    minMaxLoc(result, nullptr, &maxVal, nullptr, &maxLoc);

    t.score = (float)maxVal;
    if (maxVal < matchThreshold) return false;

    // new box top-left in frame coordinates
    Rect newBox(search.x + maxLoc.x, search.y + maxLoc.y, tw, th);
    Point2f newPos(newBox.x + tw / 2.0f, newBox.y + th / 2.0f);

    t.velocity = newPos - t.pos;
    t.pos = newPos;
    t.box = newBox;

    // adapt the template only when the match is confident, to follow slow
    // appearance changes without drifting onto the background
    if (maxVal >= updateThreshold)
    {
	Mat patch = frame(newBox);
	if (patch.size() == t.templ.size())
	    addWeighted(t.templ, 1.0 - updateRate, patch, updateRate, 0.0, t.templ);
    }

    return true;
}

// CSRT backend: the tracker does the local search and adaptation internally
bool PatternTracker::TrackCSRT (Target& t, const Mat& frame)
{
    if (!t.tracker) return false;

    Rect box = t.box;
    bool ok = false;
    try
    {
	ok = t.tracker->update(frame, box);
    }
    catch (const cv::Exception& e)
    {
	cerr << "PatternTracker: CSRT update failed: " << e.what() << endl;
	return false;
    }
    if (!ok) { t.score = 0.0f; return false; }

    Point2f newPos(box.x + box.width / 2.0f, box.y + box.height / 2.0f);

    // reject an update that jumps further than the allowed search distance
    float dx = newPos.x - t.pos.x;
    float dy = newPos.y - t.pos.y;
    if (sqrt(dx * dx + dy * dy) > maxDistance + max(box.width, box.height))
    {
	t.score = 0.0f;
	return false;
    }

    t.velocity = newPos - t.pos;
    t.pos = newPos;
    t.box = box & Rect(0, 0, frame.cols, frame.rows);
    t.score = 1.0f;
    return true;
}

void PatternTracker::Apply()
{
    Mat frame = pipeline->frame;
    if (frame.empty()) return;

    if (detectionMask.size() != frame.size())
	detectionMask = Mat::zeros(frame.rows, frame.cols, CV_8U);

    // label the incoming foreground mask so boxes can be taken from the blobs
    if (fitToMask)
	ComputeComponents(pipeline->marked);

    // rebuild backend state for existing targets if the backend was switched
    if (backendChanged)
    {
	for (auto& t : targets)
	{
	    if (!t.active) continue;
	    if (backend == CSRT)
	    {
		t.templ.release();
		try { t.tracker = TrackerCSRT::create(); t.tracker->init(frame, t.box); }
		catch (...) { t.active = false; }
	    }
	    else
	    {
		t.tracker.release();
		Rect b = t.box & Rect(0, 0, frame.cols, frame.rows);
		if (b.width >= 8 && b.height >= 8) frame(b).copyTo(t.templ);
		else t.active = false;
	    }
	}
	backendChanged = false;
    }

    // 1. manual seeds queued from the GUI (processed even while paused). The
    // box is taken from the mask blob under the click so it is tight and
    // correctly shaped; if the click misses every blob, fall back to a square.
    for (auto& p : pendingSeeds)
    {
	Rect blobBox;
	Point2f blobCentroid;
	if (fitToMask && ComponentBoxNear(p, maxDistance, blobBox, blobCentroid))
	    SeedTarget(blobBox, frame);
	else
	    SeedTarget(BoxAround(Point2f(p.x, p.y), templateSize, frame), frame);
    }
    pendingSeeds.clear();

    // 2. automatic seeds from the foreground mask blobs
    if (seedFromDetection && fitToMask)
    {
	// one target per sufficiently large mask blob not already tracked
	const int frameArea = pipeline->width * pipeline->height;
	for (int lab = 1; lab < ccCount; lab++)
	{
	    int area = ccStats.at<int>(lab, CC_STAT_AREA);
	    if (area < minBlobSeedSize) continue;
	    // skip a blob covering most of the frame: this is not a target but an
	    // unsettled background (e.g. before a subtractor has learned the scene)
	    if (area > frameArea / 2) continue;
	    Point2f cen((float)ccCentroids.at<double>(lab, 0),
			(float)ccCentroids.at<double>(lab, 1));
	    if (NearExistingTarget(cen, (float)maxDistance)) continue;
	    Rect box(ccStats.at<int>(lab, CC_STAT_LEFT),
		     ccStats.at<int>(lab, CC_STAT_TOP),
		     ccStats.at<int>(lab, CC_STAT_WIDTH),
		     ccStats.at<int>(lab, CC_STAT_HEIGHT));
	    SeedTarget(box, frame);
	}
    }
    else if (seedFromDetection)
    {
	// legacy path (fit-to-mask off): square box sized from the blob area
	vector<Blob>& blobs = pipeline->parent->blobs;
	for (auto& b : blobs)
	{
	    if (!b.available) continue;
	    if ((int)b.size < minBlobSeedSize) continue;
	    Point2f bp(b.x, b.y);
	    if (NearExistingTarget(bp, (float)maxDistance)) continue;
	    int side = (int)round(sqrt((double)b.size) * 1.5);
	    side = max(templateSize / 2, min(side, 256));
	    SeedTarget(BoxAround(bp, side, frame), frame);
	}
    }

    // 3. advance existing targets, but only on a real new frame; on a static
    // frame (paused / replayed) keep the last positions
    if (!pipeline->parent->staticFrame)
    {
	for (auto& t : targets)
	{
	    if (!t.active) continue;

	    bool found = (backend == CSRT) ? TrackCSRT(t, frame)
					   : TrackTemplate(t, frame);

	    if (found)
	    {
		t.lostFrames = 0;
		t.trail.push_back(Point((int)t.pos.x, (int)t.pos.y));
		while ((int)t.trail.size() > trailLength) t.trail.pop_front();
	    }
	    else
	    {
		t.lostFrames++;
		// coast on the last known velocity while briefly lost
		if (usePrediction) t.pos += t.velocity;
		if (t.lostFrames > maxLostFrames) t.active = false;
	    }

	    // refit the reported box to the underlying mask blob (rate-limited).
	    // On a frame with no blob it keeps the last size and follows pos.
	    if (fitToMask && t.active)
		RefitBoxToMask(t);

	    // drop a target whose center has left the frame
	    if (t.pos.x < 0 || t.pos.x >= frame.cols ||
		t.pos.y < 0 || t.pos.y >= frame.rows)
		t.active = false;
	}

	// drop inactive targets
	targets.erase(remove_if(targets.begin(), targets.end(),
				[](const Target& t) { return !t.active; }),
		      targets.end());
    }

    // 4. stamp the tracked targets into the marked mask
    detectionMask.setTo(0);
    for (auto& t : targets)
    {
	if (!t.active) continue;
	Point center((int)t.pos.x, (int)t.pos.y);
	Size axes(max(1, t.box.width / 2), max(1, t.box.height / 2));
	ellipse(detectionMask, center, axes, 0, 0, 360, Scalar(255), FILLED);
    }

    if (additive)
	pipeline->marked |= (detectionMask & pipeline->zoneMap);
    else
	pipeline->marked &= detectionMask;
}

void PatternTracker::OutputHud (Mat& hud)
{
    for (auto& t : targets)
    {
	if (!t.active) continue;

	Scalar color(0, 200, 255, 255);

	// trail
	for (size_t i = 1; i < t.trail.size(); i++)
	    line(hud, t.trail[i - 1], t.trail[i], Scalar(80, 80, 80, 255), 1, LINE_AA);

	rectangle(hud, t.box, color, 2, LINE_AA);

	char label[64];
	snprintf(label, sizeof(label), "#%d %.2f", t.id, t.score);
	putText(hud, label, Point(t.box.x, max(12, t.box.y - 4)),
		FONT_HERSHEY_SIMPLEX, 0.5, color, 1, LINE_AA);
    }
}

void PatternTracker::OpenOutput()
{
    if (outputStream.is_open()) outputStream.close();
    if (!outputFilename.empty())
    {
	outputStream.open(outputFilename.c_str(), ios::out);
	if (outputStream.is_open())
	    outputStream << "time \t frame \t target_id \t x \t y \t width \t height \t score" << endl;
    }
}

void PatternTracker::CloseOutput()
{
    if (outputStream.is_open()) outputStream.close();
}

void PatternTracker::OutputStep()
{
    if (!outputStream.is_open()) return;

    double time = pipeline->parent->GetPresentTime();
    long frame = pipeline->parent->GetPresentFrameNumber();

    for (auto& t : targets)
    {
	if (!t.active) continue;
	outputStream
	    << time << "\t" << frame << "\t" << t.id << "\t"
	    << (int)t.pos.x << "\t" << (int)t.pos.y << "\t"
	    << t.box.width << "\t" << t.box.height << "\t"
	    << t.score << endl;
    }
}

void PatternTracker::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	output = (int)fn["Output"];
	if (!fn["Backend"].empty()) backend = (int)fn["Backend"];
	if (!fn["MaxDistance"].empty()) maxDistance = (int)fn["MaxDistance"];
	if (!fn["TemplateSize"].empty()) templateSize = (int)fn["TemplateSize"];
	if (!fn["FitToMask"].empty()) fitToMask = (int)fn["FitToMask"];
	if (!fn["SizeAdaptRate"].empty()) sizeAdaptRate = (float)fn["SizeAdaptRate"];
	if (!fn["MatchThreshold"].empty()) matchThreshold = (float)fn["MatchThreshold"];
	if (!fn["UpdateThreshold"].empty()) updateThreshold = (float)fn["UpdateThreshold"];
	if (!fn["UpdateRate"].empty()) updateRate = (float)fn["UpdateRate"];
	if (!fn["MaxLostFrames"].empty()) maxLostFrames = (int)fn["MaxLostFrames"];
	if (!fn["MaxTargets"].empty()) maxTargets = (int)fn["MaxTargets"];
	if (!fn["UsePrediction"].empty()) usePrediction = (int)fn["UsePrediction"];
	if (!fn["TrailLength"].empty()) trailLength = (int)fn["TrailLength"];
	if (!fn["SeedFromDetection"].empty()) seedFromDetection = (int)fn["SeedFromDetection"];
	if (!fn["MinBlobSeedSize"].empty()) minBlobSeedSize = (int)fn["MinBlobSeedSize"];
	additive = (int)fn["Additive"];
	outputFilename = (string)fn["OutputFilename"];

	if (backend != TEMPLATE && backend != CSRT) backend = TEMPLATE;
	if (maxDistance < 1) maxDistance = 40;
	if (templateSize < 8) templateSize = 48;
	if (maxTargets < 1) maxTargets = 10;
    }
}

void PatternTracker::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Output" << output;
    fs << "Backend" << backend;
    fs << "MaxDistance" << maxDistance;
    fs << "TemplateSize" << templateSize;
    fs << "FitToMask" << fitToMask;
    fs << "SizeAdaptRate" << sizeAdaptRate;
    fs << "MatchThreshold" << matchThreshold;
    fs << "UpdateThreshold" << updateThreshold;
    fs << "UpdateRate" << updateRate;
    fs << "MaxLostFrames" << maxLostFrames;
    fs << "MaxTargets" << maxTargets;
    fs << "UsePrediction" << usePrediction;
    fs << "TrailLength" << trailLength;
    fs << "SeedFromDetection" << seedFromDetection;
    fs << "MinBlobSeedSize" << minBlobSeedSize;
    fs << "Additive" << additive;
    fs << "OutputFilename" << outputFilename;
}
