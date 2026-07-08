
#ifndef IMAGE_PROCESSING_ENGINE_H
#define IMAGE_PROCESSING_ENGINE_H


#include "PipelinePlugin.h"
#include "Parameters.h"
#include "Capture.h"

#include <vector>
#include <deque>
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

    // prefetch buffer: decoded frames around the current playback position.
    // The decoder is kept "prefetchAhead" frames ahead of the playhead, so
    // temporal plugins can read future frames, and recent past frames are
    // kept for fast step-back. When prefetchAhead is 0 the playhead follows
    // the decoder (no latency) and only past frames are cached.
    struct BufferedFrame { long number = 0; double time = 0; cv::Mat image; };
    std::deque<BufferedFrame> frameBuffer;
    int playIndex = -1;         // process head: the frame the pipeline runs on
    int prefetchAhead = 0;      // future frames kept decoded (frame look-ahead)
    int outputLatency = 0;      // frames the present head lags the process head
    int maxPastFrames = 15;
    long presentNumber = 0;     // displayed frame (process head - outputLatency)
    double presentTime = 0.0;

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

    // zones of interest (mask image path; polygon ROIs live in the
    // ZonesOfInterest plugin, which rasterizes into zoneMap)
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

    // prefetch buffer / playback control
    int ComputePrefetch();                 // max PrefetchAhead over active plugins
    int ComputeOutputLatency();            // max OutputLatency over active plugins
    bool DecodeOne();                       // decode the next frame into the buffer
    void PresentPlayhead();                 // set the process/present frames
    bool AdvanceFrame();                    // step the playhead forward one frame
    bool StepBackward();                    // step back one frame (cached if possible)
    void SeekTime(double t);                // seek + rebuild the buffer
    cv::Mat GetBufferedImage(int offset);   // frame at process head+offset (for plugins)
    cv::Mat GetPresentImage();              // image to display (process head - latency)
    // process head number: the frame the pipeline runs on (for staticFrame)
    long GetProcessFrameNumber() { return (playIndex >= 0 && playIndex < (int)frameBuffer.size()) ? frameBuffer[playIndex].number : capture->GetFrameNumber(); }
    // present (displayed) frame number / time
    long GetPresentFrameNumber() { return frameBuffer.empty() ? capture->GetFrameNumber() : presentNumber; }
    double GetPresentTime() { return frameBuffer.empty() ? capture->GetTime() : presentTime; }

    // about pipeline and threads
    void PipelineThread (unsigned int id);
    void SetupThreads ();
    void PushBack (std::vector<PipelinePlugin*> pfv, bool reset = false);
    void Insert (int pos, std::vector<PipelinePlugin*> pfv, bool reset = false);
    std::vector<PipelinePlugin*> Erase (unsigned int pos);
};

#endif
