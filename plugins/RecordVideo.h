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

#ifndef RECORD_VIDEO_H
#define RECORD_VIDEO_H

#include "PipelinePlugin.h"

#include <opencv2/imgproc/imgproc.hpp>

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
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/avutil.h>
#include <libavutil/mathematics.h>
//#include <libavutil/frame.h>
}


class RecordVideo : public PipelinePlugin
{
public:

    AVFrame* frame;
    unsigned char endcode[4] = { 0, 0, 1, 0xb7 };

    AVFormatContext* format_context;
    AVOutputFormat* output_format;
    AVCodecContext* codec_context;
    AVCodec* codec;
    AVStream* video_stream;
    int video_stream_idx;
    int input_pix_fmt;

    AVFrame* frameBGR;
    uint8_t* buffer;

    FILE* file;
    int frameCount;

    std::string outputFilename;
    std::string preset = "medium";
    bool outputOpened = false;
    int bitrate = 250;


    RecordVideo();
    ~RecordVideo();

    void OutputStep();
    void OpenOutput();
    void CloseOutput();

    void Apply();
    void Reset();
    void LoadXML (cv::FileNode& fn);
    void SaveXML (cv::FileStorage& fs);
};

#endif
