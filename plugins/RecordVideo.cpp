
#include "RecordVideo.h"


#include <opencv2/highgui/highgui.hpp>
#include "ImageProcessingEngine.h"

#include <iostream>

using namespace cv;
using namespace std;

RecordVideo::RecordVideo() : PipelinePlugin()
{
    av_register_all();
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
//    av_log_set_level(AV_LOG_VERBOSE);


    output_format = av_guess_format(NULL, outputFilename.c_str(), NULL);
    if (!output_format) {
	cerr << "Could not deduce output format from file extension: using MPEG." << endl;
//        printf("Could not deduce output format from file extension: using MPEG.\n");
        output_format = av_guess_format("mpeg", NULL, NULL);
    }
    if (!output_format) {
	cerr << "Could not find suitable output format" << endl;
        return;
    }
    /* Allocate the output media context. */
    format_context = avformat_alloc_context();
    if (!format_context) {
        cerr << "Memory error" << endl;
        return;
    }
    format_context->oformat = output_format;


    // FFMPEG STYLE
    // create output context
    // avformat_alloc_output_context2(&format_context, NULL, NULL, outputFilename.c_str());
    // if (!format_context) {
    // 	cerr << "Could not deduce output format from file extension: using MPEG." << endl;
    // 	avformat_alloc_output_context2(&format_context, NULL, "mpeg", outputFilename.c_str());
    // }

//////    avformat_open_input(&format_context, outputFilename.c_str(), NULL, NULL);
    // if (!format_context)
    // {
    // 	cerr << "Could not create a format context" << endl;
    // 	return;
    // }
    // output_format = format_context->oformat;

    /* find the video encoder */
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
    	cerr << "H264 codec not found\n" << endl;
    	return;
    }

     // color or BW
     if (pipeline->frame.channels() == 3)
	 input_pix_fmt = PIX_FMT_BGR24;
     else
	 input_pix_fmt = PIX_FMT_GRAY8;

     //create video stream
     video_stream = avformat_new_stream(format_context, codec);
     if (!video_stream) {
	 cerr << "Could not allocate stream" << endl;
	 return;
     }
     video_stream->id = format_context->nb_streams-1;
     video_stream->start_time = 0;
     codec_context = video_stream->codec;

     // video stream
     codec_context->codec_id = AV_CODEC_ID_H264;

     av_opt_set(codec_context->priv_data, "preset", preset.c_str(), 0);

    /* put sample parameters */
    codec_context->bit_rate = bitrate * 1000 * 8;

    /* resolution must be a multiple of two */
    codec_context->width = pipeline->width;
    codec_context->height = pipeline->height;

    /* frames per second */
//    codec_context->gop_size = 120; /* emit one intra frame every X frames */
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
    if(codec && codec->supported_framerates)
    {
        const AVRational *p= codec->supported_framerates;
        AVRational req = {frame_rate, frame_rate_base};
        const AVRational *best=NULL;
        AVRational best_error= {INT_MAX, 1};
        for(; p->den!=0; p++)
    	{
            AVRational error= av_sub_q(req, *p);
            if(error.num <0) error.num *= -1;
            if(av_cmp_q(error, best_error) < 0)
    	    {
                best_error= error;
                best= p;
            }
        }
        codec_context->time_base.den= best->num;
        codec_context->time_base.num= best->den;
	video_stream->time_base = codec_context->time_base;
    }

    /* Some formats want stream headers to be separate. */
    if (format_context->oformat->flags & AVFMT_GLOBALHEADER)
    	codec_context->flags |= CODEC_FLAG_GLOBAL_HEADER;

    /* open it */
    if (avcodec_open2(codec_context, codec, NULL) < 0) {
    	fprintf(stderr, "Could not open codec\n");
    	exit(1);
    }

    //frame = alloc_picture(codec_context->pix_fmt, pipeline->width, pipeline->height);

    // FFMPEG STYLE
    frame = avcodec_alloc_frame();
//    frame = av_frame_alloc();
    frame->format = codec_context->pix_fmt;
    frame->width  = pipeline->width;
    frame->height = pipeline->height;
    frame->pts = 0;
    frame->pkt_pts = 0;
    frame->pkt_dts = 0;

    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    int ret = av_image_alloc(frame->data, frame->linesize, pipeline->width, pipeline->height, codec_context->pix_fmt, 32);
    if (ret < 0) {
    	cerr << "Could not allocate raw picture buffer" << endl;
    	return;
    }

    frameBGR=avcodec_alloc_frame();
//    frameBGR=av_frame_alloc();
    int numBytes=avpicture_get_size(PIX_FMT_RGB24, pipeline->width, pipeline->height);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    memset (buffer, 0, numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *)frameBGR, buffer, PIX_FMT_RGB24, pipeline->width, pipeline->height);

    // av_dump_format(format_context, 0, outputFilename.c_str(), 1);

    /* open the output file, if needed */
    if (!(output_format->flags & AVFMT_NOFILE)) {
        if (avio_open(&format_context->pb, outputFilename.c_str(), AVIO_FLAG_WRITE) < 0) {
            cerr << "Could not open video output file " << outputFilename << endl;
            return;
        }
    }

    /* Write the stream header, if any. */
    avformat_write_header(format_context, NULL);

    outputOpened = true;
}

void RecordVideo::OutputStep ()
{
    if (!outputOpened) return;

    // copy mat to frame
    for (int y = 0; y < pipeline->height; y++)
    {
	unsigned char* row = (unsigned char*) frameBGR->data[0] + y * frameBGR->linesize[0];
	unsigned char* mrow = pipeline->frame.ptr<uchar>(y);
	memcpy (row, mrow, pipeline->width*3);
    }

    static struct SwsContext *img_convert_ctx = NULL;

    // convert frame from RGB to YUV ?
    if (img_convert_ctx == NULL)
    {
	img_convert_ctx = sws_getContext(
	    pipeline->width, pipeline->height, PIX_FMT_BGR24,
	    pipeline->width, pipeline->height, codec_context->pix_fmt,
	    SWS_FAST_BILINEAR, NULL, NULL, NULL);
    }
    sws_scale(img_convert_ctx, frameBGR->data, frameBGR->linesize, 0, pipeline->height, frame->data, frame->linesize);

    AVPacket packet;
    av_init_packet(&packet);
    packet.data = NULL;    // packet data will be allocated by the encoder
    packet.size = 0;

    // PTS calculation
    double pts = pipeline->parent->capture->GetTime() * pipeline->parent->capture->fps;
    unsigned long lpts = round(pts);

    if (lpts > frameCount) 
    {
	cout << "Skipping a frame, from/to : " << frameCount << " " << lpts << endl;
	frameCount = lpts;
    }

    frame->pts = frameCount;
    frameCount++;

    /* encode the image */
    int got_output;
    int ret = avcodec_encode_video2(codec_context, &packet, frame, &got_output);
    if (ret < 0) {
	cerr << "Error encoding frame" << endl;
	return;
    }

    if (got_output)
    {
	packet.pts = av_rescale_q(packet.pts, codec_context->time_base, video_stream->time_base);
	packet.dts = av_rescale_q(packet.dts, codec_context->time_base, video_stream->time_base);

	// FFMPEG STYLE
//	av_packet_rescale_ts(&packet, codec_context->time_base, video_stream->time_base);
	ret = av_interleaved_write_frame(format_context, &packet);
    }
}


void RecordVideo::CloseOutput ()
{
    if (!outputOpened) return;

    av_write_trailer(format_context);

    /* close the output file */
    if (!(output_format->flags & AVFMT_NOFILE))
    {
    	avio_close(format_context->pb);
    }

    /* free the streams */
    for(unsigned int i = 0; i < format_context->nb_streams; i++) {
    	av_freep(&format_context->streams[i]->codec);
    	av_freep(&format_context->streams[i]);
    }

    /* free the stream */
    av_free(format_context);

    // free the frame
    av_freep(&frame->data[0]);
    avcodec_free_frame(&frame);
//    av_frame_free(&frame);
    av_freep(&frameBGR->data[0]);
    avcodec_free_frame(&frameBGR);
//    av_frame_free(&frameBGR);

//    av_free(buffer);

    outputOpened = false;
}


void RecordVideo::Reset()
{
//    if (output) OpenOutput();
}


void RecordVideo::Apply()
{
}

void RecordVideo::LoadXML (FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	output = (int)fn["Output"];
	outputFilename = (string)fn["OutputFilename"];
	preset = (string)fn["Preset"];
	bitrate = (int)fn["Bitrate"];
    }
}

void RecordVideo::SaveXML (FileStorage& fs)
{
    fs << "Active" << active;
    fs << "Output" << output;
    fs << "OutputFilename" << outputFilename;
    fs << "Preset" << preset;
    fs << "Bitrate" << (int)bitrate;
}



