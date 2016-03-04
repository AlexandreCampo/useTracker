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

#include "CaptureVideo.h"

#include <iostream>
#include <wx/time.h>
#include <cmath>

using namespace std;
using namespace cv;


CaptureVideo::CaptureVideo(string filename) : Capture()
{
    if (Open (filename))
	type = VIDEO;
}

CaptureVideo::CaptureVideo(FileNode& fn) : Capture()
{
    LoadXML (fn);
    if (Open (filename))
	type = VIDEO;
}

CaptureVideo::~CaptureVideo()
{
    Close();
}

string CaptureVideo::GetName()
{
    return filename;
}

bool CaptureVideo::Open (string filename)
{
    // endcode[0] = 0;
    // endcode[1] = 0;
    // endcode[2] = 1;
    // endcode[3] = 0xb7;
        


    // open the video file
    this->filename = filename;

    memset(&avpacket, 0, sizeof(avpacket));
    av_init_packet (&avpacket);
    av_register_all();
	    	    
//	av_log_set_level(AV_LOG_FATAL);
    av_log_set_level(AV_LOG_VERBOSE);
    
    // Open video file
    if (avformat_open_input(&format_context, filename.c_str(), NULL, NULL) < 0)
    {
	cerr << "Error : Could not open video file " << filename << endl;
	fprintf(stderr, "Could not open video file %s\n", filename.c_str());
	return 0;
//	exit (-1); // Couldn't open file
    }
    
    // Retrieve stream information
    if (avformat_find_stream_info(format_context, NULL) < 0) 
    {
	cerr << "Error : Could not find stream information in video file " << filename << endl;
	//fprintf(stderr, "Could not find stream information in video file %s\n", filename.c_str());
	return 0;
	//exit( -1); // Couldn't find stream information
    }
  
    video_stream_idx = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_stream_idx < 0) 
	return 0;
    //exit( -1);

    video_stream = format_context->streams[video_stream_idx];
    if (!video_stream) 
    {
	cerr << "Error : Could not find video stream in the input, aborting" << endl; 
//	fprintf(stderr, "Could not find video stream in the input, aborting\n");
	return 0;
//	    exit( -1);
    }

    /* find decoder for the stream */
    codec_context = video_stream->codec;
    codec = avcodec_find_decoder(codec_context->codec_id);
    if (!codec) 
    {
	cerr << "Error : Failed to find codec" << endl;
//	fprintf(stderr, "Failed to find %s codec\n", av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
	return 0;
//	    exit( ret);
    }
    if ((ret = avcodec_open2(codec_context, codec, NULL)) < 0) 
    {
	cerr << "Error : Failed to open codec" << endl;
//	fprintf(stderr, "Failed to open %s codec\n", av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
	return 0;
//	    exit( ret);
    }
    
    // Dump information about file onto standard error
    // if (verbose)
    // {
    // 	av_dump_format(format_context, 0, videoFilename, 0);
    // }

    avframe = avcodec_alloc_frame();
//    frameBGR = avcodec_alloc_frame();
    if (video_stream->avg_frame_rate.den > 0)
	fps = av_q2d(video_stream->avg_frame_rate);
    else 
	fps = av_q2d(video_stream->r_frame_rate);
    
//    fpsi = 1.0 / av_q2d(video_stream->time_base);


//	cout << "Video FPS, FPSI " << fps << " " << fpsi << " den " << video_stream->avg_frame_rate.den << " r framerate " << av_q2d(video_stream->r_frame_rate) << " time base " << av_q2d(video_stream->time_base) << " start time " << video_stream->start_time << endl;

    // Determine required buffer size and allocate buffer
    width = codec_context->width;
    height = codec_context->height;

    memset( &frameBGR, 0, sizeof(frameBGR) );
    
    numBytes=avpicture_get_size(PIX_FMT_BGR24, width, height);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    memset (buffer, 0, numBytes * sizeof(uint8_t));

    // prepare context for conversion to opencv Mat
    img_convert_ctx = sws_getContext(width, height, 
				     codec_context->pix_fmt, 
				     width, height, PIX_FMT_BGR24, SWS_FAST_BILINEAR,
				     NULL, NULL, NULL);
    
    // Assign appropriate parts of buffer to image planes in frameRGB
    // Note that frameRGB is an AVFrame, but AVFrame is a superset of AVPicture

    frame = Mat::zeros (height, width, CV_8UC3); 
//    frameBGR.data[0] = (uint8_t *)frame.data;
    avpicture_fill((AVPicture *)&frameBGR, frameBGR.data[0], PIX_FMT_BGR24, width, height);
    
//    Rewind();
    
    GrabFrame();
    ConvertFrame();

//    firstFrameNumber = frameNumber;
    long pts;

    if (avpacket.pts != AV_NOPTS_VALUE && avpacket.pts != 0)
	pts = avpacket.pts;
    else
	pts = avpacket.dts;

    cout << "Init : first pts = " << pts << " pkt pts = " << avpacket.pts << " dts = " << avpacket.dts << " start time = " << video_stream->start_time << endl;
    firstFramePTS = pts;
    
    frameNumber = 0;

    playSpeed = 0;
    playTimestep.Assign(1000000.0 / fps);
    isPaused = true;

    cout << "detected w/h/fps " << width << " " << height << " " << fps << std::endl;

    // read first frame to allow display

//    frame = Mat();
  
//    source >> frame;
    // if (!GrabFrame ())
    // 	return false;
    // ConvertFrame();
    // GetFrameNumber();

    calibration.Undistort(frame);


/*
    source.open (filename.c_str());
    if (!source.isOpened())
    {
	std::cerr << "Could not open video source" << std::endl;
	return false;
    }

    width = source.get(CV_CAP_PROP_FRAME_WIDTH);
    height = source.get(CV_CAP_PROP_FRAME_HEIGHT);
    fps = source.get(CV_CAP_PROP_FPS);

    // fps incorrectly detected
    if (fps <= 0.000001 || std::isnan(fps)) fps = 1;

    playSpeed = 0;
    playTimestep.Assign(1000000.0 / fps);
    isPaused = true;

    cout << "detected w/h/fps " << width << " " << height << " " << fps << std::endl;

    // read first frame to allow display
    Mat prevFrame = frame;
    source >> frame;
    calibration.Undistort(frame);
*/


    return true;
    //return (!frame.empty());
}

void CaptureVideo::Close ()
{
    // Free the YUV and BGR frames
    av_free(avframe);
//    av_free(frameBGR);
    
    // Close the codec
    if (codec_context) avcodec_close(codec_context);

    // close convert context
    if( img_convert_ctx ) sws_freeContext(img_convert_ctx);

    // Close the video file
    avformat_close_input(&format_context);
}

bool CaptureVideo::GrabFrame ()
{
    int frameFinished; 
    
    // free packet if previously allocated 
    av_free_packet (&avpacket);
    
    while (1) 
    {
	int ret = av_read_frame(format_context, &avpacket);

	if (ret < 0) break;
        if (ret == AVERROR(EAGAIN)) continue;
	
	// Is this a packet from the video stream?
	if(avpacket.stream_index == video_stream_idx) 
	{
	    // Decode video frame
	    avcodec_decode_video2(codec_context, avframe, &frameFinished, &avpacket);
	    
	    // Did we get a video frame?
	    if(frameFinished) 
	    {
		cout << "grab : pts/dts = " << avpacket.pts << ":" << avpacket.dts << " first = " << firstFramePTS << " tb = " << video_stream->time_base.num << ":" << video_stream->time_base.den << endl; 
		return true;
	    }
	}
    }

    return false;
}

bool CaptureVideo::ConvertFrame ()
{
    // prepare context for conversion to opencv Mat
    img_convert_ctx = sws_getContext(width, height, 
    				     codec_context->pix_fmt, 
    				     width, height, PIX_FMT_BGR24, SWS_FAST_BILINEAR,
    				     NULL, NULL, NULL);

//    int aligns[AV_NUM_DATA_POINTERS];
//   avcodec_align_dimensions2(codec_context, &width, &height, aligns);
    frameBGR.data[0] = (uint8_t*)realloc(frameBGR.data[0], avpicture_get_size( AV_PIX_FMT_BGR24, width, height));
    avpicture_fill( (AVPicture*)&frameBGR, frameBGR.data[0], AV_PIX_FMT_BGR24, width, height );


    // av_frame_unref(&frameBGR);
    // frameBGR.format = AV_PIX_FMT_BGR24;
    // frameBGR.width = width;
    // frameBGR.height = height;
    // if (0 != av_frame_get_buffer(&frameBGR, 32))
    // {
    // 	cerr << "Error : Out of memory, cannot allocate new conversion frame" << endl;
    // 	return false;
    // }

    // Convert the image from its native format to BGR opencv Mat
    sws_scale(img_convert_ctx, avframe->data, avframe->linesize, 0, height, frameBGR.data, frameBGR.linesize);
  
    // just copy to mat
    memcpy (frame.data, frameBGR.data[0], frame.step[0] * height);

//    frame.data = frameBGR->data[0];
//    frame.step[0] = frameBGR->linesize[0];
//    frame = Mat (height, width, CV_8UC3, frameBGR->data[0], frameBGR->linesize[0]);

    return true;
}


bool CaptureVideo::GetNextFrame ()
{
    if (!GrabFrame ())
    {
	nextFrameTime += playTimestep;    
	return false;
    }

    ConvertFrame();
    frameNumber++;
    calibration.Undistort(frame);

    return true;
}

wxLongLong CaptureVideo::GetNextFrameSystemTime()
{
    return nextFrameTime;
}

void CaptureVideo::Pause()
{
    isPaused = true;
    statusChanged = true;
}

void CaptureVideo::Play()
{
    // restart timing
    if (isPaused || isStopped)
    {
	nextFrameTime = wxGetUTCTimeUSec() + playTimestep;
	isPaused = false;
	isStopped = false;
	statusChanged = true;
    }
}

long CaptureVideo::RecalculateFrameNumberFromTimestamp()
{
    long pts;

    if (avpacket.pts != AV_NOPTS_VALUE && avpacket.pts != 0)
	pts = avpacket.pts;
    else 
	pts = avpacket.dts;

    double time = (double)(pts - firstFramePTS) * r2d(video_stream->time_base);

    // long int ts = av_frame_get_best_effort_timestamp(frame);
    // double time = double(ts - video_stream->start_time) / fpsi;

    frameNumber = (long)(fps * time + 0.5);

    // TODO DEBUG
    cout << "Recalculate from timestamp : pts = " << pts << "pkt= " << avpacket.pts << ":" <<avpacket.dts << " start time = " << video_stream->start_time << " frame=" << frameNumber << " time = " << time << endl;

    return frameNumber;
}

long CaptureVideo::GetFrameNumber ()
{
    // frameNumber = (long)(fps * GetTime() + 0.5);

    // TODO DEBUG
//    cout << "Get frame number = " << frameNumber << endl;

    return frameNumber;
}

double CaptureVideo::GetTime ()
{
//     long pts;

//     if (avpacket.pts != AV_NOPTS_VALUE && avpacket.pts != 0)
// 	pts = avpacket.pts;
//     else
// 	pts = avpacket.dts;
    
// //    pts -= firstFrameNumber;

//     double time = (double)(pts - video_stream->start_time) * r2d(video_stream->time_base);

    double time = (double) frameNumber / fps;

    // TODO DEBUG
//    cout << "Get time : pts = " << pts << "pkt= " << avpacket.pts << ":" <<avpacket.dts << " start time = " << video_stream->start_time << " time = " << time << endl;

    // long int ts = av_frame_get_best_effort_timestamp(frame);
    // double time = double(ts - video_stream->start_time) / fpsi;
    return time;
}

void CaptureVideo::Rewind ()
{
    av_seek_frame(format_context, video_stream_idx, firstFramePTS, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
    avcodec_flush_buffers(codec_context);
    GrabFrame();
    ConvertFrame();
//    GetFrameNumber();
    frameNumber = 0;
}


bool CaptureVideo::GetFrame (double time)
{
    // calculate desired timestamp
    long int desiredFrame = long(time * fps) - firstFramePTS;

    return SetFrameNumber(desiredFrame);
}

bool CaptureVideo::GetPreviousFrame()
{
    int f = GetFrameNumber();
    bool ok = SetFrameNumber(f-1);

    nextFrameTime += playTimestep;

    return ok;
}

void CaptureVideo::Stop()
{
    Rewind();
    calibration.Undistort(frame);

    isStopped = true;
    statusChanged = true;
}

bool CaptureVideo::SetFrameNumber(long desiredFrame)
{
    // get current frame
    if (desiredFrame < 0) desiredFrame = 0;

    cout << "Seek 0 : going from frame " << frameNumber << " to frame " << desiredFrame << endl;
        
    // first try to use seek, if goal is far away
//    if (abs(desiredFrame - frameNumber) > fps * 10)
//    for (int i = 0; i < 4; i++)
    {
	// int flags = AVSEEK_FLAG_ANY;

	// if (frameNumber > desiredFrame) 
	//     flags |= AVSEEK_FLAG_BACKWARD;

	int flags = AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY;
	
	long targetPTS = firstFramePTS + long(desiredFrame / fps / r2d(video_stream->time_base) + 0.5);
	cout << "Seek 1 : asking for PTS " << targetPTS << endl;
	av_seek_frame(format_context, video_stream_idx, targetPTS, flags);
	avcodec_flush_buffers(codec_context);
	GrabFrame();
	// GetFrameNumber();
	RecalculateFrameNumberFromTimestamp();
//	currentFrame = GetFrameTimestamp ();
	    cout << "Seek 2 : is now at frame " << frameNumber << endl;
    }

    // still ahead of desired frame ? 
    if (frameNumber > desiredFrame) 
    {
	av_seek_frame(format_context, video_stream_idx, firstFramePTS, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
	avcodec_flush_buffers(codec_context);
	GrabFrame();
    }
    cout << "Seek 2.5 : is now at frame " << frameNumber << endl;

	
    // now go forward until desired frame is reached
    int currentFrame = frameNumber;
    while (currentFrame < desiredFrame)
//    for (int i = 0; i < desiredFrame - frameNumber; i++) 
    {
	if (!GrabFrame())
	    break;
	// if (GetFrameTimestamp() >= desiredFrame) break; // tried hard to match stuff with mariano...
	cout << "Seek 3 : is now at frame " << currentFrame << endl;
	currentFrame++;
    }
//	while (GetFrameTimestamp() < desiredFrame) GetNextFrame();
    frameNumber = currentFrame;
    cout << "Seek 4 : is now at frame " << frameNumber << endl;
   
//    source.set(CV_CAP_PROP_POS_MSEC, time * 1000.0);

    ConvertFrame();
//    GetFrameNumber();

    // Mat previousFrame = frame;
    // source >> frame;
    calibration.Undistort(frame);

    nextFrameTime += playTimestep;
    statusChanged = true;

    // TODO always true ? 
    return true;


    // source.set(CV_CAP_PROP_POS_FRAMES, frameNumber);

    // Mat previousFrame = frame;    
    // source >> frame;
    // calibration.Undistort(frame);

    // statusChanged = true;

    // nextFrameTime += playTimestep;
    // GetFrameNumber();

    // if (frame.empty())
    // {
    // 	frame = previousFrame;
    // 	return false;
    // }
    // return true;
}

// adapted from opencv
long CaptureVideo::GetFrameCount ()
{
    long nbf = video_stream->nb_frames;

    // did not work ? estimate from sec duration
    if (nbf == 0)
    {
	double sec = (double)format_context->duration / (double)AV_TIME_BASE;
	
	if (sec < 0.000025)
	{
	    sec = (double)video_stream->duration * r2d(video_stream->time_base);
	}
	
	if (sec < 0.000025)
	{
	    sec = (double)video_stream->duration * r2d(video_stream->time_base);
	}
	
        nbf = (long)floor(sec * fps + 0.5);
    }
    return nbf;
}

// from opencv
double CaptureVideo::r2d(AVRational r) const
{
    return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

void CaptureVideo::SetSpeedFaster(int speed)
{
    if (speed > 1) playTimestep.Assign(1000000.0 / (fps * speed));
    else playTimestep.Assign(1000000.0 / fps);
    nextFrameTime = wxGetUTCTimeUSec() + playTimestep;
}

void CaptureVideo::SetSpeedSlower(int speed)
{
    if (speed > 1) playTimestep.Assign(1000000.0 * speed / fps);
    else playTimestep.Assign(1000000.0 / fps);
    nextFrameTime = wxGetUTCTimeUSec() + playTimestep;
}

void CaptureVideo::SaveXML(FileStorage& fs)
{
    fs << "Type" << "video";
    fs << "Filename" << filename;

    if (calibration.calibrated)
    {
	fs << "Calibration" << "{";

	calibration.SaveXML(fs);

	fs << "}";
    }
}

void CaptureVideo::LoadXML(FileNode& fn)
{
    if (!fn.empty())
    {
	filename = (string)fn["Filename"];

	FileNode calibNode = fn ["Calibration"];
	if (!calibNode.empty())
	{
	    calibration.LoadXML (calibNode);
	}
    }
}
