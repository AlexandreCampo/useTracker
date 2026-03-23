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

#ifndef CAPTURE_VIDEO_H
#define CAPTURE_VIDEO_H

#include "Capture.h"

extern "C"
{
#include <libavutil/common.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
}

struct CaptureVideo : Capture
{
    std::string filename;

    long frameNumber = 0;
    int64_t startTime = 0;
    int64_t nextFrameTime = 0;
    int64_t time = 0;
    int64_t playSpeedMul = 1;   // delay multiplier (for slower playback)
    int64_t playSpeedDiv = 1;   // delay divisor (for faster playback)


    CaptureVideo(std::string filename);
    CaptureVideo(cv::FileNode& fn);
    ~CaptureVideo();

    bool Open(std::string filename);
    void Close();

    bool GetNextFrame ();
    bool GetPreviousFrame();
    int64_t GetNextFrameSystemTime();

    bool GetFrame (double time);

    void Stop();

    void Play();
    void Pause();

    void SetSpeedFaster(int speed) override;
    void SetSpeedSlower(int speed) override;
    void SetPlaySpeed(int level) override;

    long GetFrameNumber();
    long GetFrameCount();
    bool SetFrameNumber(long f);

    void SetTime(double time);
    double GetTime();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    std::string GetName();

    // internal libav
    AVFormatContext* format_context = nullptr;
    AVCodecContext* codec_context = nullptr;
    const AVCodec* codec = nullptr;
    AVStream* video_stream = nullptr;
    int video_stream_idx = -1;
    AVPixelFormat  pixel_format;

    struct SwsContext* img_convert_ctx = nullptr;

    AVFrame* avframe = nullptr;
    AVPacket* avpacket = nullptr;

    int ret;

    AVFrame* frameBGR = nullptr;
    int numBytes;
    uint8_t* buffer = nullptr;

    long firstPts = 0;
    long nextPts = 0;
    long currentPts = 0;
    long deltaPts = 0;
    int64_t frameDelay = 0;

    bool GrabFrame();
    bool ConvertFrame();
    double r2d(AVRational r) const;
    void Rewind();
    double GetFrameTime ();
    long RecalculateFrameNumberFromTimestamp();
    double GetDuration();
    void EstimateFrameTimings();
    void SeekTimestamp(long ts);
};




#endif //CAPTURE_VIDEO_H
