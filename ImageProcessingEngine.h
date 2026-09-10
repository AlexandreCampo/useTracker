
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
    // image = raw decoded frame (read by temporal plugins); processed = the
    // frame after the pipeline ran on it (e.g. enhanced), used to display the
    // delayed present frame in sync with a centered plugin's mask
    struct BufferedFrame { long number = 0; double time = 0; cv::Mat image; cv::Mat processed; };
    std::deque<BufferedFrame> frameBuffer;
    int playIndex = -1;         // process head: the frame the pipeline runs on
    int prefetchAhead = 0;      // future frames kept decoded (frame look-ahead)
    int outputLatency = 0;      // frames the present head lags the process head
    int maxPastFrames = 45;     // cached past frames for fast step-back
    long presentNumber = 0;     // displayed frame (process head - outputLatency)
    double presentTime = 0.0;

    // progressive backward refill (driven one batch per frame so a real
    // progress bar can be shown while the chunk is re-read from disk)
    bool refilling = false;
    long refillTarget = 0;      // frame the process head should land on
    long refillStart = 0;       // first frame of the chunk being decoded
    long refillNeedUpTo = 0;    // decode until this frame number is buffered
    // second phase: when a centered plugin delays the display, the freshly
    // decoded chunk has no cached processed frames, so it is re-processed
    // forward to repopulate them (and rebuild temporal plugin state)
    bool refillProcessing = false;
    int  refillProcessIdx = 0;  // next buffer index to (re)process
    int  refillTargetIdx = 0;   // buffer index of the target frame

    // input downscaling: the pipeline runs on frames scaled by inputScale
    // (1.0 = full resolution). Downscaling speeds up parameter tuning. All
    // internal buffers and plugin coordinates are in this scaled space;
    // GetOutputScale() converts them back to source-resolution for output.
    // backgroundNative / zoneMapNative hold the source-resolution originals so
    // the scale can be changed without losing them.
    float inputScale = 1.0f;
    int procWidth = 0;
    int procHeight = 0;
    cv::Mat backgroundNative;
    cv::Mat zoneMapNative;
    // stable processing frame at proc resolution: the pipeline slices view into
    // this (not the decoder's capture->frame, which stays at source size). Kept
    // a fixed buffer so the per-thread slice views stay valid between frames.
    cv::Mat procFrame;

    void UpdateProcSize();
    void BuildProcBuffers();          // (re)allocate buffers/slices at proc size
    void CaptureNativeBgZone();       // stash native background/zone before scaling
    void SetInputScale(float s);      // change scale and rebuild (keeps position)
    int  ProcWidth()  const { return procWidth; }
    int  ProcHeight() const { return procHeight; }
    // factor to convert scaled (internal) coordinates back to source resolution
    double GetOutputScale() const { return (inputScale > 1e-6f) ? 1.0 / (double)inputScale : 1.0; }

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
    void RefreshCurrentFrame();             // show the already decoded frame after opening/resetting
    void PresentPlayhead();                 // set the process/present frames
    bool AdvanceFrame();                    // step the playhead forward one frame
    bool StepBackward();                    // step back one frame (cached if possible)
    // step back is cached: true when the previous frame is already buffered
    bool CanStepBackwardCached() { return playIndex > 0; }
    // rebuild the buffer as a chunk ending at targetFrame (one seek + prefetch)
    void RefillBackward(long targetFrame);
    // progressive variant: begin, then pump a batch per frame until done
    void BeginRefillBackward(long targetFrame);
    bool PumpRefill(int batch);             // returns true when the chunk is ready
    void FinishRefill();                    // land the process head on the target
    float RefillProgress();                 // 0..1 fraction of the chunk ready
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
