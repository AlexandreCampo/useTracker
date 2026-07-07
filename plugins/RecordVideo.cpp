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

#include "RecordVideo.h"


#include <opencv2/highgui.hpp>
#include "ImageProcessingEngine.h"

#include <iostream>

RecordVideo::RecordVideo() : PipelinePlugin()
{
}

RecordVideo::~RecordVideo()
{
    CloseOutput();
}

void RecordVideo::OpenOutput ()
{
    // close output if it is already opened
    if (outputOpened) CloseOutput();

    // now init a new video
    frameCount = 0;

    av_log_set_level(AV_LOG_FATAL);

    const AVOutputFormat* output_format = av_guess_format(NULL, outputFilename.c_str(), NULL);
    if (!output_format) {
	std::cerr << "Could not deduce output format from file extension: using MPEG." << std::endl;
        output_format = av_guess_format("mpeg", NULL, NULL);
    }
    if (!output_format) {
	std::cerr << "Could not find suitable output format" << std::endl;
        return;
    }
    this->output_format = output_format;

    /* Allocate the output media context. */
    format_context = avformat_alloc_context();
    if (!format_context) {
        std::cerr << "Memory error" << std::endl;
        return;
    }
    format_context->oformat = output_format;

    /* find the video encoder */
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
    	std::cerr << "H264 codec not found" << std::endl;
    	return;
    }

     // color or BW
     if (pipeline->frame.channels() == 3)
	 input_pix_fmt = AV_PIX_FMT_BGR24;
     else
	 input_pix_fmt = AV_PIX_FMT_GRAY8;

     //create video stream
     video_stream = avformat_new_stream(format_context, codec);
     if (!video_stream) {
	 std::cerr << "Could not allocate stream" << std::endl;
	 return;
     }
     video_stream->id = format_context->nb_streams-1;
     video_stream->start_time = 0;

     // create a separate codec context
     codec_context = avcodec_alloc_context3(codec);
     if (!codec_context) {
	 std::cerr << "Could not allocate codec context" << std::endl;
	 return;
     }

     // video stream
     codec_context->codec_id = AV_CODEC_ID_H264;

     av_opt_set(codec_context->priv_data, "preset", preset.c_str(), 0);

    /* put sample parameters */
    codec_context->bit_rate = bitrate * 1000 * 8;

    /* resolution must be a multiple of two */
    codec_context->width = pipeline->width;
    codec_context->height = pipeline->height;

    /* frames per second */
    codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    int frame_rate=(int)(pipeline->parent->capture->fps+0.5);
    int frame_rate_base = 1;
    while (fabs((double)frame_rate/frame_rate_base) - pipeline->parent->capture->fps > 0.001){
        frame_rate_base *= 10;
        frame_rate = (int)(pipeline->parent->capture->fps * frame_rate_base + 0.5);
    }
    codec_context->time_base.den = frame_rate;
    codec_context->time_base.num = frame_rate_base;
    video_stream->time_base = codec_context->time_base;
    /* adjust time base for supported framerates */
    if(codec)
    {
        const AVRational *p = NULL;
        int count = 0;
        bool found = false;

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(61, 0, 0)
        found = (avcodec_get_supported_config(codec_context, codec,
                AV_CODEC_CONFIG_FRAME_RATE, 0,
                (const void **)&p, &count) == 0 && p && count > 0);
#else
        if (codec->supported_framerates)
        {
            p = codec->supported_framerates;
            for (count = 0; p[count].den != 0; count++) {}
            found = (count > 0);
        }
#endif

        if (found)
        {
            AVRational req = {frame_rate, frame_rate_base};
            const AVRational *best = NULL;
            AVRational best_error = {INT_MAX, 1};
            for(int i = 0; i < count; i++)
            {
                AVRational error = av_sub_q(req, p[i]);
                if(error.num < 0) error.num *= -1;
                if(av_cmp_q(error, best_error) < 0)
                {
                    best_error = error;
                    best = &p[i];
                }
            }
            if (best)
            {
                codec_context->time_base.den = best->num;
                codec_context->time_base.num = best->den;
                video_stream->time_base = codec_context->time_base;
            }
        }
    }

    /* Some formats want stream headers to be separate. */
    if (format_context->oformat->flags & AVFMT_GLOBALHEADER)
    	codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    /* open it */
    if (avcodec_open2(codec_context, codec, NULL) < 0) {
    	std::cerr << "Could not open codec" << std::endl;
    	return;
    }

    /* copy codec parameters to the stream */
    avcodec_parameters_from_context(video_stream->codecpar, codec_context);

    frame = av_frame_alloc();
    frame->format = codec_context->pix_fmt;
    frame->width  = pipeline->width;
    frame->height = pipeline->height;
    frame->pts = 0;

    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    int ret = av_image_alloc(frame->data, frame->linesize, pipeline->width, pipeline->height, codec_context->pix_fmt, 32);
    if (ret < 0) {
    	std::cerr << "Could not allocate raw picture buffer" << std::endl;
    	return;
    }

    frameBGR = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pipeline->width, pipeline->height, 1);
    buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    memset (buffer, 0, numBytes * sizeof(uint8_t));
    av_image_fill_arrays(frameBGR->data, frameBGR->linesize, buffer, AV_PIX_FMT_RGB24, pipeline->width, pipeline->height, 1);

    /* open the output file, if needed */
    if (!(output_format->flags & AVFMT_NOFILE)) {
        if (avio_open(&format_context->pb, outputFilename.c_str(), AVIO_FLAG_WRITE) < 0) {
            std::cerr << "Could not open video output file " << outputFilename << std::endl;
            return;
        }
    }

    /* Write the stream header, if any. */
    int ret2 = avformat_write_header(format_context, NULL);
    if (ret2 < 0) {
        std::cerr << "Could not write video header" << std::endl;
        return;
    }

    outputOpened = true;
}

void RecordVideo::OutputStep ()
{
    if (!outputOpened) return;

    // record the pristine source frame: enhancement plugins may have
    // modified the capture frame in place
    cv::Mat src = pipeline->frame;
    if (!pipeline->parent->sourceFrame.empty()
	&& pipeline->parent->sourceFrame.size() == cv::Size(pipeline->parent->capture->width, pipeline->parent->capture->height))
	src = pipeline->parent->sourceFrame(pipeline->roi);

    // copy mat to frame
    for (int y = 0; y < pipeline->height; y++)
    {
	unsigned char* row = (unsigned char*) frameBGR->data[0] + y * frameBGR->linesize[0];
	const unsigned char* mrow = src.ptr<uchar>(y);
	memcpy (row, mrow, pipeline->width*3);
    }

    static struct SwsContext *img_convert_ctx = NULL;

    // convert frame from RGB to YUV ?
    if (img_convert_ctx == NULL)
    {
	img_convert_ctx = sws_getContext(
	    pipeline->width, pipeline->height, AV_PIX_FMT_BGR24,
	    pipeline->width, pipeline->height, codec_context->pix_fmt,
	    SWS_FAST_BILINEAR, NULL, NULL, NULL);
    }
    sws_scale(img_convert_ctx, frameBGR->data, frameBGR->linesize, 0, pipeline->height, frame->data, frame->linesize);

    // PTS calculation
    double pts = pipeline->parent->capture->GetTime() * pipeline->parent->capture->fps;
    int lpts = round(pts);

    if (lpts > frameCount)
    {
	frameCount = lpts;
    }

    frame->pts = frameCount;
    frameCount++;

    /* encode the image */
    AVPacket* pkt = av_packet_alloc();
    int ret = avcodec_send_frame(codec_context, frame);
    while (ret >= 0) {
	ret = avcodec_receive_packet(codec_context, pkt);
	if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
	if (ret < 0) { std::cerr << "Error encoding frame" << std::endl; break; }
	av_packet_rescale_ts(pkt, codec_context->time_base, video_stream->time_base);
	av_interleaved_write_frame(format_context, pkt);
	av_packet_unref(pkt);
    }
    av_packet_free(&pkt);
}


void RecordVideo::CloseOutput ()
{
    if (!outputOpened) return;

    // flush encoder by sending NULL frame
    avcodec_send_frame(codec_context, nullptr);
    AVPacket* pkt = av_packet_alloc();
    while (avcodec_receive_packet(codec_context, pkt) == 0) {
	av_packet_rescale_ts(pkt, codec_context->time_base, video_stream->time_base);
	av_interleaved_write_frame(format_context, pkt);
	av_packet_unref(pkt);
    }
    av_packet_free(&pkt);

    av_write_trailer(format_context);

    /* close the output file */
    if (!(output_format->flags & AVFMT_NOFILE))
    {
    	avio_close(format_context->pb);
    }

    /* free the format context and its streams */
    avformat_free_context(format_context);

    /* free the codec context */
    avcodec_free_context(&codec_context);

    // free the frame
    av_freep(&frame->data[0]);
    av_frame_free(&frame);

    av_freep(&frameBGR->data[0]);
    av_frame_free(&frameBGR);

    outputOpened = false;
}


void RecordVideo::Reset()
{
}


void RecordVideo::Apply()
{
}

void RecordVideo::LoadXML (cv::FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	output = (int)fn["Output"];
	outputFilename = (std::string)fn["OutputFilename"];
	preset = (std::string)fn["Preset"];
	bitrate = (int)fn["Bitrate"];
    }
}

void RecordVideo::SaveXML (cv::FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Output" << output;
    fs << "OutputFilename" << outputFilename;
    fs << "Preset" << preset;
    fs << "Bitrate" << (int)bitrate;
}
