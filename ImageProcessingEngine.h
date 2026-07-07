
#ifndef IMAGE_PROCESSING_ENGINE_H
#define IMAGE_PROCESSING_ENGINE_H


#include "PipelinePlugin.h"
#include "Parameters.h"
#include "Capture.h"

#include <vector>
#include <iostream>

#include <thread>
#include <mutex>

#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>



class Blob;

struct ImageProcessingEngine
{
    // about source
    Capture* capture = nullptr;

    /* // about frames */
    int frameNumber = 0;
    int lastFrameNumber = 0;
    bool staticFrame = true;

    // about image processing
    cv::Mat background;
    cv::Mat zoneMap;
    cv::Mat marked;
    cv::Mat labels;
    // pristine copy of the capture frame, saved before the pipeline runs:
    // enhancement plugins (CLAHE, white balance, ...) modify the capture
    // frame in place; the copy is restored on static re-steps so they stay
    // idempotent, and RecordVideo uses it to record the original images
    cv::Mat sourceFrame;
    std::vector<cv::KeyPoint> keypoints;
    std::vector<Blob> blobs;

    // about visualisation
    cv::Mat pipelineSnapshot;
    cv::Mat pipelineSnapshotMarked;
    bool takeSnapshot;
    unsigned int snapshotPos;
    cv::Mat hud;

    // about threads
    unsigned int threadsCount = 0;
    std::vector<std::thread> threads;
    std::vector<std::mutex*> threadsStart; // unlocked by ipEngine exclusively
    std::vector<std::mutex*> threadsDone;  // unlocked by threads exclusively
    std::vector<std::mutex*> threadsPause; // unlocked by special thread exclusively
    std::vector<std::mutex*> threadsRestart; // unlocked by threads exclusively
    bool threadsStop;
    bool threadsDrawHud;
    bool threadsOutput;
    std::vector<Pipeline> pipelines;

    // about time :-)
    /* float currentTime; */

    // ...parameters
    float startTime = 0.0;
    float durationTime = 0.0;
    float timestep = 0.0;
    float nextStepTime = 0.0;
    bool useTimeBoundaries = false;


    // background
    enum BgCalcType {BG_MEAN, BG_MEDIAN};
    BgCalcType bgCalcType = BG_MEAN;
    std::string bgFilename;
    bool bgRecalculate = false;
    int bgFrames = 11;
    float bgEndTime = 0.0;
    float bgStartTime = 0.0;
    unsigned char bgLowThreshold = 0;
    unsigned char bgHighThreshold = 255;

    // zones of interest
    std::string zonesFilename;

    // output
    bool output = false;

    // main operation
    ImageProcessingEngine ();
    ~ImageProcessingEngine ();
    void Reset (Parameters& parameters);
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
    void Step (bool drawHud = false);
    bool GetNextFrame();
    void OpenOutput();
    void CloseOutput();

    // about pipeline and threads
    void PipelineThread (unsigned int id);
    void SetupThreads ();
    void PushBack (std::vector<PipelinePlugin*> pfv, bool reset = false);
    void Insert (int pos, std::vector<PipelinePlugin*> pfv, bool reset = false);
    std::vector<PipelinePlugin*> Erase (unsigned int pos);
};

#endif
