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
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#endif


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
//    cv::VideoCapture source;
//    cv::Mat frame;

    long firstFramePTS = 0;
    long frameNumber = 0;
    wxLongLong startTime = 0;
    wxLongLong nextFrameTime = 0;
    wxLongLong playTimestep = 0;
    wxLongLong time = 0;
    int playSpeed = 0;


    CaptureVideo(std::string filename);
    CaptureVideo(cv::FileNode& fn);
    ~CaptureVideo();

    bool Open(std::string filename);
    void Close();

    bool GetNextFrame ();
    bool GetPreviousFrame();
    wxLongLong GetNextFrameSystemTime();

    bool GetFrame (double time);

//    void Start();
    void Stop();

    void Play();
    void Pause();

    void SetSpeedFaster(int speed);
    void SetSpeedSlower(int speed);

    long GetFrameNumber();
    long GetFrameCount();
    bool SetFrameNumber(long f);

    void SetTime(double time);
    double GetTime();

    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);

    std::string GetName();

    // internal libav
    AVFormatContext* format_context = NULL;
    AVCodecContext* codec_context = NULL;
    AVCodec* codec = NULL;
    AVStream* video_stream = NULL;
    int video_stream_idx = -1;

    struct SwsContext* img_convert_ctx = NULL;

//    AVOutputFormat* output_format = NULL;

    AVFrame* avframe = NULL;
    AVPacket avpacket;

    int ret;

    AVFrame frameBGR;
    int numBytes;
    uint8_t* buffer = NULL;

    /* int width; */
    /* int height; */
    /* float fps; */
    /* double fpsi; // internal fps, counts can be weird and is specific to the encoded file */
    
//    int frameCount;
    unsigned char endcode[4] = {0, 0, 1, 0xb7};

//    int verbose;

    bool GrabFrame();
    bool ConvertFrame();
    double r2d(AVRational r) const;
    void Rewind();
    double GetFrameTime ();
    long RecalculateFrameNumberFromTimestamp();

};




#endif //CAPTURE_VIDEO_H
