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

#include "YoloDetector.h"

#include "ImageProcessingEngine.h"

#include <opencv2/core/ocl.hpp>

#include <iostream>
#include <sstream>
#include <algorithm>

using namespace cv;
using namespace std;

YoloDetector::YoloDetector() : PipelinePlugin()
{
    // deep inference uses global image context and its own threading,
    // run it once on the full frame rather than per pipeline slice
    multithreaded = false;
}

YoloDetector::~YoloDetector()
{
    CloseOutput();
}

void YoloDetector::Reset()
{
    detectionMask = Mat::zeros(pipeline->height, pipeline->width, CV_8U);
    detections.clear();
    cacheValid = false;

    // (re)load the model and class names declared in the settings
    LoadModel();
    LoadClassNames();
}

void YoloDetector::LoadModel()
{
    netLoaded = false;
    loadedModelFilename.clear();
    triedModelFilename = modelFilename; // remember the attempt (success or not)
    cacheValid = false;                 // force a fresh inference

    if (modelFilename.empty())
    {
	status = "no model loaded";
	return;
    }

    try
    {
	net = dnn::readNet(modelFilename);
	if (net.empty())
	{
	    status = "failed to load model";
	    return;
	}
	netLoaded = true;
	loadedModelFilename = modelFilename;
	// apply the selected compute target (GPU if available, else CPU)
	ApplyTarget();
    }
    catch (const cv::Exception& e)
    {
	netLoaded = false;
	status = string("error: ") + e.what();
	cerr << "YoloDetector: " << e.what() << endl;
    }
}

string YoloDetector::TargetName(int t)
{
    switch (t)
    {
    case TARGET_OPENCL:      return "OpenCL";
    case TARGET_OPENCL_FP16: return "OpenCL FP16";
    case TARGET_VULKAN:      return "Vulkan";
    default:                 return "CPU";
    }
}

// configure the net for target t and run one warmup inference; returns false
// if the target is unavailable or the model cannot run on it
bool YoloDetector::TryTarget(int t)
{
    if (!netLoaded) return false;

    // OpenCL needs a usable runtime/device, checked up front for a clean message
    if ((t == TARGET_OPENCL || t == TARGET_OPENCL_FP16) && !ocl::haveOpenCL())
	return false;

    int backend = dnn::DNN_BACKEND_OPENCV;
    int tgt = dnn::DNN_TARGET_CPU;
    switch (t)
    {
    case TARGET_OPENCL:      backend = dnn::DNN_BACKEND_OPENCV; tgt = dnn::DNN_TARGET_OPENCL; break;
    case TARGET_OPENCL_FP16: backend = dnn::DNN_BACKEND_OPENCV; tgt = dnn::DNN_TARGET_OPENCL_FP16; break;
    case TARGET_VULKAN:      backend = dnn::DNN_BACKEND_VKCOM;  tgt = dnn::DNN_TARGET_VULKAN; break;
    default:                 backend = dnn::DNN_BACKEND_OPENCV; tgt = dnn::DNN_TARGET_CPU; break;
    }

    try
    {
	net.setPreferableBackend(backend);
	net.setPreferableTarget(tgt);

	// warmup: forces kernel compilation and surfaces unsupported-layer
	// errors now rather than mid-playback
	Mat dummy = Mat::zeros(inputSize, inputSize, CV_8UC3);
	Mat blob;
	dnn::blobFromImage(dummy, blob, 1.0 / 255.0, Size(inputSize, inputSize),
			   Scalar(), true, false);
	net.setInput(blob);
	vector<Mat> outs;
	net.forward(outs, net.getUnconnectedOutLayersNames());
	return true;
    }
    catch (const cv::Exception& e)
    {
	cerr << "YoloDetector: target " << TargetName(t) << " unavailable: "
	     << e.what() << endl;
	return false;
    }
}

void YoloDetector::ApplyTarget()
{
    if (!netLoaded) return;

    cacheValid = false; // the target change requires a fresh inference

    if (TryTarget(target))
    {
	activeTarget = target;
	status = "model loaded (target: " + TargetName(target) + ")";
	return;
    }

    // requested target failed: fall back to the CPU
    if (target != TARGET_CPU && TryTarget(TARGET_CPU))
    {
	activeTarget = TARGET_CPU;
	status = "model loaded (" + TargetName(target) +
		 " unavailable, using CPU)";
	return;
    }

    activeTarget = TARGET_CPU;
    status = "model loaded (target: CPU)";
}

void YoloDetector::LoadClassNames()
{
    classNames.clear();
    cacheValid = false; // labels affect the drawn/decoded output
    if (classNamesFilename.empty()) return;

    ifstream f(classNamesFilename);
    if (!f.is_open())
    {
	cerr << "YoloDetector: could not open class names file " << classNamesFilename << endl;
	return;
    }

    string line;
    while (getline(f, line))
    {
	// strip trailing carriage return / whitespace
	while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' '))
	    line.pop_back();
	if (!line.empty())
	    classNames.push_back(line);
    }
}

string YoloDetector::GetClassName(int classId)
{
    if (classId >= 0 && classId < (int)classNames.size())
	return classNames[classId];
    return to_string(classId);
}

bool YoloDetector::ClassAllowed(int classId)
{
    if (classFilter.empty()) return true;

    // the filter is a comma separated list of class names or numeric ids
    string name = GetClassName(classId);
    string idStr = to_string(classId);

    stringstream ss(classFilter);
    string token;
    while (getline(ss, token, ','))
    {
	// trim spaces
	size_t a = token.find_first_not_of(" \t");
	size_t b = token.find_last_not_of(" \t");
	if (a == string::npos) continue;
	token = token.substr(a, b - a + 1);

	if (token == idStr) return true;
	// case insensitive name match
	string lt = token, ln = name;
	transform(lt.begin(), lt.end(), lt.begin(), ::tolower);
	transform(ln.begin(), ln.end(), ln.begin(), ::tolower);
	if (lt == ln) return true;
    }
    return false;
}

void YoloDetector::RunInference()
{
    detections.clear();

    if (!netLoaded || pipeline->frame.empty()) return;

    Mat frame = pipeline->frame;
    int fw = frame.cols;
    int fh = frame.rows;

    // ---- letterbox: resize preserving aspect ratio, pad to a square ----
    float scale = min((float)inputSize / fw, (float)inputSize / fh);
    int newW = (int)round(fw * scale);
    int newH = (int)round(fh * scale);
    int padX = (inputSize - newW) / 2;
    int padY = (inputSize - newH) / 2;

    Mat resized;
    resize(frame, resized, Size(newW, newH));
    Mat input(inputSize, inputSize, frame.type(), Scalar(114, 114, 114));
    resized.copyTo(input(Rect(padX, padY, newW, newH)));

    Mat blob;
    dnn::blobFromImage(input, blob, 1.0 / 255.0, Size(inputSize, inputSize),
		       Scalar(), true, false);
    net.setInput(blob);

    vector<Mat> outputs;
    try
    {
	net.forward(outputs, net.getUnconnectedOutLayersNames());
    }
    catch (const cv::Exception& e)
    {
	status = string("inference error: ") + e.what();
	return;
    }
    if (outputs.empty()) return;

    // Pick the detection tensor. A segmentation model (YOLOv8/YOLOE-seg) emits
    // two outputs: the detection head (3D: 1 x A x B) and a 4D prototype-mask
    // tensor (1 x 32 x H x W). Prefer a 3D output so we never decode the masks.
    Mat out = outputs[0];
    for (const Mat& o : outputs)
	if (o.dims == 3) { out = o; break; }

    // ---- decode: support both YOLOv5 and YOLOv8/v11 layouts ----
    // The raw output is a 3D tensor 1 x A x B. YOLOv5 has A = num boxes and
    // each row is [cx, cy, w, h, objectness, class scores...]. YOLOv8/v11
    // has B = num boxes and each column is [cx, cy, w, h, class scores...]
    // (no objectness). We detect the orientation and presence of objectness
    // from the tensor shape.
    if (out.dims == 3)
	out = out.reshape(1, out.size[1]); // A x B

    int rows = out.rows;
    int cols = out.cols;

    // decide orientation: the smaller dimension is the number of attributes
    bool transposed = false;
    int numBoxes = rows;
    int numAttrs = cols;
    if (rows < cols)
    {
	// attributes are along the rows (YOLOv8 exported as 1 x (4+nc) x boxes)
	transposed = true;
	numBoxes = cols;
	numAttrs = rows;
	out = out.t();
    }

    // YOLOv5: numAttrs == 5 + nc, has an objectness score at index 4
    // YOLOv8/v11: numAttrs == 4 + nc, no objectness
    // AUTO infers it from the tensor orientation (the canonical Ultralytics
    // exports put v5 as boxes x attrs and v8 as attrs x boxes), otherwise the
    // user forces the layout to handle non standard converters.
    bool hasObjectness;
    if (modelType == V5)      hasObjectness = true;
    else if (modelType == V8) hasObjectness = false;
    else                      hasObjectness = (!transposed);

    int classOffset = hasObjectness ? 5 : 4;
    int numClasses = numAttrs - classOffset;
    if (numClasses <= 0)
    {
	// fall back to the other interpretation
	hasObjectness = !hasObjectness;
	classOffset = hasObjectness ? 5 : 4;
	numClasses = numAttrs - classOffset;
	if (numClasses <= 0) return;
    }

    // If a class-names list is provided and the tensor carries MORE class
    // channels than names, the extra trailing channels are not classes: for an
    // instance-segmentation model (YOLOv8/YOLOE-seg) the layout is
    // [box(4), classes(nc), mask_coeffs(32)], so capping to the known class
    // count reads the real class scores and ignores the mask coefficients,
    // letting a seg model be used as a plain detector.
    if (!classNames.empty() && (int)classNames.size() < numClasses)
	numClasses = (int)classNames.size();

    vector<Rect> boxes;
    vector<float> confidences;
    vector<int> classIds;

    const float* data = (const float*)out.data;
    for (int i = 0; i < numBoxes; i++)
    {
	const float* row = data + i * numAttrs;

	float objectness = hasObjectness ? row[4] : 1.0f;
	if (objectness < confidenceThreshold) continue;

	// best class
	int bestClass = -1;
	float bestScore = 0.0f;
	for (int c = 0; c < numClasses; c++)
	{
	    float s = row[classOffset + c];
	    if (s > bestScore) { bestScore = s; bestClass = c; }
	}

	float confidence = bestScore * objectness;
	if (confidence < confidenceThreshold) continue;
	if (!ClassAllowed(bestClass)) continue;

	// box is in letterboxed input coordinates: undo the letterbox
	float cx = row[0];
	float cy = row[1];
	float w  = row[2];
	float h  = row[3];

	float x0 = (cx - w / 2.0f - padX) / scale;
	float y0 = (cy - h / 2.0f - padY) / scale;
	float bw = w / scale;
	float bh = h / scale;

	boxes.push_back(Rect((int)round(x0), (int)round(y0), (int)round(bw), (int)round(bh)));
	confidences.push_back(confidence);
	classIds.push_back(bestClass);
    }

    // ---- non maximum suppression ----
    vector<int> keep;
    dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, keep);

    for (int idx : keep)
    {
	Detection d;
	d.box = boxes[idx] & Rect(0, 0, fw, fh); // clip to frame
	d.classId = classIds[idx];
	d.confidence = confidences[idx];
	if (d.box.width > 0 && d.box.height > 0)
	    detections.push_back(d);
    }

    stringstream st;
    st << "model loaded, " << detections.size() << " detection(s)";
    status = st.str();
}

void YoloDetector::Apply()
{
    // (re)load the model when the filename changes, but do not retry the same
    // filename every frame if the load failed (avoids per-frame disk churn)
    if (modelFilename != loadedModelFilename && modelFilename != triedModelFilename)
	LoadModel();

    if (detectionMask.size() != Size(pipeline->width, pipeline->height))
	detectionMask = Mat::zeros(pipeline->height, pipeline->width, CV_8U);

    // inference is expensive: skip it on a static frame (paused / replayed)
    // and reuse the previous detections
    if (!pipeline->parent->staticFrame || !cacheValid)
    {
	RunInference();
	cacheValid = true;
    }

    // build a mask of the detections, so that ExtractBlobs / Tracker can use
    // the deep detections just like any other segmentation result
    detectionMask.setTo(0);
    for (auto& d : detections)
    {
	// filled ellipse inscribed in the detection box
	Point center(d.box.x + d.box.width / 2, d.box.y + d.box.height / 2);
	Size axes(max(1, d.box.width / 2), max(1, d.box.height / 2));
	ellipse(detectionMask, center, axes, 0, 0, 360, Scalar(255), FILLED);
    }

    if (additive)
	pipeline->marked |= (detectionMask & pipeline->zoneMap);
    else
	pipeline->marked &= detectionMask;
}

void YoloDetector::OutputHud (Mat& hud)
{
    double os = pipeline->parent->GetOutputScale();
    auto SP  = [&](cv::Point p){ return cv::Point(cvRound(p.x*os), cvRound(p.y*os)); };
    auto SR  = [&](cv::Rect r){ return cv::Rect(cvRound(r.x*os), cvRound(r.y*os), cvRound(r.width*os), cvRound(r.height*os)); };

    for (auto& d : detections)
    {
	Scalar color(0, 220, 0, 255);
	rectangle(hud, SR(d.box), color, 2, LINE_AA);

	char label[128];
	snprintf(label, sizeof(label), "%s %.0f%%",
		 GetClassName(d.classId).c_str(), d.confidence * 100.0f);

	int baseline = 0;
	Size ts = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
	Point org(d.box.x, max(ts.height + 2, d.box.y - 4));
	// scale only the anchor position; text-size-derived dimensions stay unscaled
	Point orgS = SP(org);
	rectangle(hud, Rect(orgS.x, orgS.y - ts.height - 2, ts.width + 4, ts.height + 6),
		  Scalar(0, 0, 0, 200), FILLED);
	putText(hud, label, Point(orgS.x + 2, orgS.y),
		FONT_HERSHEY_SIMPLEX, 0.5, color, 1, LINE_AA);
    }
}

void YoloDetector::OpenOutput()
{
    if (outputStream.is_open()) outputStream.close();

    if (!outputFilename.empty())
    {
	outputStream.open(outputFilename.c_str(), ios::out);
	if (outputStream.is_open())
	    outputStream << "time \t frame \t class_id \t class_name \t confidence \t x \t y \t width \t height" << endl;
    }
}

void YoloDetector::CloseOutput()
{
    if (outputStream.is_open()) outputStream.close();
}

void YoloDetector::OutputStep()
{
    if (!outputStream.is_open()) return;

    double time = pipeline->parent->GetPresentTime();
    long frame = pipeline->parent->GetPresentFrameNumber();

    for (auto& d : detections)
    {
	outputStream
	    << time << "\t"
	    << frame << "\t"
	    << d.classId << "\t"
	    << GetClassName(d.classId) << "\t"
	    << d.confidence << "\t"
	    << d.box.x << "\t"
	    << d.box.y << "\t"
	    << d.box.width << "\t"
	    << d.box.height << endl;
    }
}

void YoloDetector::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	output = (int)fn["Output"];
	modelFilename = (string)fn["ModelFilename"];
	classNamesFilename = (string)fn["ClassNamesFilename"];
	if (!fn["ModelType"].empty())
	    modelType = (int)fn["ModelType"];
	if (!fn["Target"].empty())
	    target = (int)fn["Target"];
	if (!fn["InputSize"].empty())
	    inputSize = (int)fn["InputSize"];
	if (!fn["ConfidenceThreshold"].empty())
	    confidenceThreshold = (float)fn["ConfidenceThreshold"];
	if (!fn["NMSThreshold"].empty())
	    nmsThreshold = (float)fn["NMSThreshold"];
	classFilter = (string)fn["ClassFilter"];
	additive = (int)fn["Additive"];
	outputFilename = (string)fn["OutputFilename"];

	if (modelType < AUTO || modelType > V8) modelType = AUTO;
	if (target < TARGET_CPU || target > TARGET_VULKAN) target = TARGET_CPU;
	if (inputSize < 32) inputSize = 640;
	if (confidenceThreshold <= 0.0f || confidenceThreshold > 1.0f) confidenceThreshold = 0.25f;
	if (nmsThreshold <= 0.0f || nmsThreshold > 1.0f) nmsThreshold = 0.45f;
    }
}

void YoloDetector::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Output" << output;
    fs << "ModelFilename" << modelFilename;
    fs << "ClassNamesFilename" << classNamesFilename;
    fs << "ModelType" << modelType;
    fs << "Target" << target;
    fs << "InputSize" << inputSize;
    fs << "ConfidenceThreshold" << confidenceThreshold;
    fs << "NMSThreshold" << nmsThreshold;
    fs << "ClassFilter" << classFilter;
    fs << "Additive" << additive;
    fs << "OutputFilename" << outputFilename;
}
