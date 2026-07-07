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

#ifndef YOLO_DETECTOR_H
#define YOLO_DETECTOR_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

#include <fstream>
#include <string>
#include <vector>
#include <set>

// Deep learning object detector based on YOLO models in ONNX format
// (YOLOv5 / v8 / v11 output layouts are detected automatically). The model
// is provided by the user; detected objects are drawn on the HUD, stamped
// into the marked mask (as filled ellipses, so that ExtractBlobs and the
// Tracker can consume them) and optionally written to a CSV file.
class YoloDetector : public PipelinePlugin
{
public:

    struct Detection
    {
	cv::Rect box;
	int classId;
	float confidence;
    };

    // output layout of the model: AUTO detects it from the tensor shape,
    // V5 forces the [x,y,w,h,objectness,classes...] layout, V8 forces the
    // [x,y,w,h,classes...] layout (also YOLOv11). Use AUTO for the standard
    // Ultralytics exports and only override for non standard converters.
    enum ModelType { AUTO = 0, V5 = 1, V8 = 2 };

    // compute target for inference. OpenCL / Vulkan use the GPU if a usable
    // runtime is present; the plugin probes the target after loading and
    // falls back to the CPU if it cannot run the model.
    enum Target { TARGET_CPU = 0, TARGET_OPENCL = 1, TARGET_OPENCL_FP16 = 2, TARGET_VULKAN = 3 };

    // parameters (saved in the settings file)
    std::string modelFilename;
    std::string classNamesFilename;
    int modelType = AUTO;
    int target = TARGET_CPU;
    int inputSize = 640;
    float confidenceThreshold = 0.25f;
    float nmsThreshold = 0.45f;
    std::string classFilter; // comma separated names or ids, empty = all
    bool additive = false;
    std::string outputFilename;

    // state
    cv::dnn::Net net;
    bool netLoaded = false;
    int activeTarget = TARGET_CPU;     // target actually running (after fallback)
    std::string loadedModelFilename;  // filename of the model in memory
    std::string triedModelFilename;   // last filename we attempted to load
    std::string status = "no model loaded";
    bool cacheValid = false;

    std::vector<std::string> classNames;
    std::vector<Detection> detections;
    cv::Mat detectionMask;
    std::ofstream outputStream;

    YoloDetector();
    ~YoloDetector();
    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    void OpenOutput();
    void CloseOutput();
    void OutputStep();
    void OutputHud (cv::Mat& hud);

    void LoadModel();
    void LoadClassNames();
    std::string GetClassName(int classId);

    // (re)apply the selected compute target to the loaded net, with warmup
    // and automatic fallback to the CPU. Safe to call when a model is loaded.
    void ApplyTarget();
    static std::string TargetName(int t);

private:
    void RunInference();
    bool ClassAllowed(int classId);
    bool TryTarget(int t);
};

#endif
