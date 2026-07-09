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

#ifndef PATTERN_TRACKER_H
#define PATTERN_TRACKER_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>

#include <fstream>
#include <string>
#include <vector>
#include <deque>

// Appearance-based (pattern) tracker. Once a target is seeded (by clicking on
// the video or automatically from a detected blob), the plugin stores the
// image patch around it and, in each following frame, searches a limited
// neighbourhood (maxDistance) for the best match to that pattern, stepping the
// target to the peak. Two matching backends are available:
//   - TEMPLATE: explicit local-window normalized cross-correlation
//               (cv::matchTemplate), with an adaptive-when-confident template
//   - CSRT:     OpenCV's discriminative correlation-filter tracker
// Tracked targets are drawn on the HUD, written to a CSV file and stamped into
// the marked mask so ExtractBlobs / Tracker can consume them.
class PatternTracker : public PipelinePlugin
{
public:

    enum Backend { TEMPLATE = 0, CSRT = 1 };

    struct Target
    {
	int id = 0;
	bool active = false;
	cv::Rect box;             // current bounding box, frame coordinates
	cv::Size2f boxSizeF;      // smoothed box size (float, rate-limited to the mask blob)
	cv::Point2f pos;          // box center
	cv::Point2f velocity;     // per frame, for search prediction
	cv::Mat templ;            // stored pattern (TEMPLATE backend)
	cv::Ptr<cv::Tracker> tracker; // CSRT backend
	float score = 0.0f;       // last match confidence
	int lostFrames = 0;
	bool confirmed = false;   // false = candidate (orange), true = live (green)
	int detectedCount = 0;    // consecutive detections toward confirmation
	int overlapFrames = 0;    // consecutive frames overlapping a stronger target
	std::deque<cv::Point> trail;
    };

    // parameters (saved in the settings file)
    int backend = TEMPLATE;
    int maxDistance = 40;          // search radius around predicted position (px)
    int templateSize = 48;         // fallback box side when no blob is under a click (px)
    bool fitToMask = true;         // derive the box from the foreground mask blob
    float sizeAdaptRate = 0.20f;   // how fast the box size follows the blob (0=frozen,1=instant)
    float matchThreshold = 0.40f;  // below this the target is considered not found
    float updateThreshold = 0.60f; // above this the template is adapted
    float updateRate = 0.10f;      // template adaptation blend factor
    int maxLostFrames = 15;        // drop a target lost for this many frames
    int maxTargets = 10;
    bool usePrediction = true;     // center the search on the predicted position
    int trailLength = 30;
    bool seedFromDetection = false;
    int minBlobSeedSize = 50;      // minimum blob area to auto-seed a target
    bool seedFromYolo = false;     // auto-seed from a YoloDetector's detections
    float yoloConfidence = 0.5f;   // minimum YOLO confidence to seed a target
    int confirmDetections = 1;     // detections needed before a candidate goes live
    bool showCandidates = true;    // draw unconfirmed candidates on the HUD
    bool mergeOverlapping = true;  // drop the weaker of two overlapping targets
    float mergeOverlap = 0.4f;     // box IoU above which two targets are merged
    int mergeDelay = 0;            // frames the overlap must persist before merging
    bool additive = false;
    std::string outputFilename;

    // state
    std::vector<Target> targets;
    int nextId = 0;
    bool backendChanged = false;
    std::vector<cv::Point> pendingSeeds; // manual seeds queued from the GUI
    cv::Mat detectionMask;
    std::ofstream outputStream;

    // connected components of the incoming foreground mask, recomputed each
    // frame; used to derive/refit target boxes from the actual blob pixels
    cv::Mat ccLabels, ccStats, ccCentroids;
    int ccCount = 0;

    PatternTracker();
    ~PatternTracker();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void OpenOutput();
    void CloseOutput();
    void OutputStep();
    void OutputHud (cv::Mat& hud);

    // called from the GUI (serialized with Apply by the pipeline barrier)
    void AddSeed (cv::Point p);
    void ClearTargets();

    void SetBackend(int b);

private:
    void SeedTarget (const cv::Rect& box, const cv::Mat& frame);
    bool TrackTemplate (Target& t, const cv::Mat& frame);
    bool TrackCSRT (Target& t, const cv::Mat& frame);
    bool NearExistingTarget (cv::Point2f p, float radius);

    // mask-blob helpers
    void ComputeComponents (const cv::Mat& mask);
    // bounding box + pixel area of the mask blob at (or nearest, within
    // searchRadius) p
    bool ComponentBoxNear (cv::Point p, int searchRadius,
			   cv::Rect& outBox, cv::Point2f& outCentroid, int& outArea);
    // resize the target box towards its underlying blob (already looked up),
    // rate-limited
    void RefitBoxToMask (Target& t, bool hasBlob, const cv::Rect& blobBox);
};

#endif
