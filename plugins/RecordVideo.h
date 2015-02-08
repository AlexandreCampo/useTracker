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
