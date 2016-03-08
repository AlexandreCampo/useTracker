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
    // open the video file
    this->filename = filename;

    memset(&avpacket, 0, sizeof(avpacket));
    av_init_packet (&avpacket);
    av_register_all();
	    	    
    av_log_set_level(AV_LOG_VERBOSE);
    
    // Open video file
    if (avformat_open_input(&format_context, filename.c_str(), NULL, NULL) < 0)
    {
	cerr << "Error : Could not open video file " << filename << endl;
	return 0;
    }
    
    // Retrieve stream information
    if (avformat_find_stream_info(format_context, NULL) < 0) 
    {
	cerr << "Error : Could not find stream information in video file " << filename << endl;
	return 0;
    }
  
    video_stream_idx = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_stream_idx < 0) 
	return 0;

    video_stream = format_context->streams[video_stream_idx];
    if (!video_stream) 
    {
	cerr << "Error : Could not find video stream in the input, aborting" << endl; 
	return 0;
    }

    /* find decoder for the stream */
    codec_context = video_stream->codec;
    codec = avcodec_find_decoder(codec_context->codec_id);
    if (!codec) 
    {
	cerr << "Error : Failed to find codec" << endl;
	return 0;
    }
    if ((ret = avcodec_open2(codec_context, codec, NULL)) < 0) 
    {
	cerr << "Error : Failed to open codec" << endl;
	return 0;
    }
    
    // av_dump_format(format_context, 0, filename.c_str(), 0);

    avframe = avcodec_alloc_frame();

    if (video_stream->avg_frame_rate.den > 0)
	fps = av_q2d(video_stream->avg_frame_rate);
    else 
	fps = av_q2d(video_stream->r_frame_rate);
    
    // Determine required buffer size and allocate buffer
    width = codec_context->width;
    height = codec_context->height;

    cout << "detected w/h/fps " << width << " " << height << " " << fps << std::endl;

    memset( &frameBGR, 0, sizeof(frameBGR) );
    
    // prepare context for conversion to opencv Mat
    img_convert_ctx = sws_getContext(width, height, 
				     codec_context->pix_fmt, 
				     width, height, PIX_FMT_BGR24, SWS_FAST_BILINEAR,
				     NULL, NULL, NULL);
    
    // Assign opencv mat buffer buffer to image planes in frameRGB
    // TODO is frameBGR linesize identical to mat linesize ?
    frame = Mat::zeros (height, width, CV_8UC3); 
    avpicture_fill((AVPicture *)&frameBGR, frame.data, PIX_FMT_BGR24, width, height);
    
    GrabFrame();
    ConvertFrame();
    calibration.Undistort(frame);

    firstPts = nextPts;    
    frameNumber = 0;
    playSpeed.Assign(1.0);
    isPaused = true;
    nextFrameTime = wxGetUTCTimeUSec();

//    cout << "Opened video : start_time " << video_stream->start_time << " firstPts " << firstPts << " fps " << fps << " nb_frames " << video_stream->nb_frames << " duration1 " << (double)format_context->duration / (double)AV_TIME_BASE << " duration2 " <<  video_stream->duration * av_q2d(video_stream->time_base) << " d() = " << GetDuration() << endl;

    return true;
}

void CaptureVideo::Close ()
{
    // Free the YUV and BGR frames
    av_free(avframe);
    
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
        
    while (1) 
    {
	// free packet if previously allocated 
	av_free_packet (&avpacket);

	// get next packet
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
		EstimateFrameTimings();
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

    avpicture_fill( (AVPicture*)&frameBGR, frame.data, AV_PIX_FMT_BGR24, width, height );

    // Convert the image from its native format to BGR opencv Mat
    sws_scale(img_convert_ctx, avframe->data, avframe->linesize, 0, height, frameBGR.data, frameBGR.linesize);
  
    return true;
}

// A new frame has been grabbed, calculate next pts
void CaptureVideo::EstimateFrameTimings()
{
    currentPts = nextPts;

    // find next pts from frame info
    if (avframe->pkt_dts != AV_NOPTS_VALUE)
    {
	nextPts = avframe->pkt_dts;
    }
    else if (avframe->pkt_pts != AV_NOPTS_VALUE)
    {
	nextPts = avframe->pkt_pts;
    }
    // no valid pts found, make a prediction instead
    else
    {
	nextPts = currentPts + deltaPts;
    }

    deltaPts = nextPts - currentPts;
    frameDelay.Assign (deltaPts * av_q2d(video_stream->time_base) * 1000000.0);
}

bool CaptureVideo::GetNextFrame ()
{
    frameNumber = RecalculateFrameNumberFromTimestamp();
    ConvertFrame();

    if (!GrabFrame ())
	return false;

//    cout << "GetNextFrame : " << " pts:dts " << avframe->pkt_pts << ":" << avframe->pkt_dts << " tb " << video_stream->time_base.num << ":" << video_stream->time_base.den << endl;
    
    calibration.Undistort(frame);

    nextFrameTime += frameDelay * playSpeed;

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
	nextFrameTime = wxGetUTCTimeUSec() + frameDelay * playSpeed;

	isPaused = false;
	isStopped = false;
	statusChanged = true;
    }
}

long CaptureVideo::RecalculateFrameNumberFromTimestamp()
{
    double time = (nextPts - firstPts) * av_q2d(video_stream->time_base);
    frameNumber = (long)(fps * time + 0.5);

    return frameNumber;
}

long CaptureVideo::GetFrameNumber ()
{
    return RecalculateFrameNumberFromTimestamp();
}

double CaptureVideo::GetTime ()
{
    return (double)(currentPts - firstPts) * av_q2d(video_stream->time_base);
}

void CaptureVideo::Rewind ()
{
    av_seek_frame(format_context, video_stream_idx, firstPts, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
    avcodec_flush_buffers(codec_context);

    long d = deltaPts;

    GrabFrame();
    
    // we jumped frames back or forward, but we try to maintain same playback
    deltaPts = d;
    frameDelay.Assign (deltaPts * av_q2d(video_stream->time_base) * 1000000.0);

    ConvertFrame();
    frameNumber = 0;
}


bool CaptureVideo::GetFrame (double desiredTime)
{       
    long targetPts = firstPts + desiredTime / av_q2d(video_stream->time_base);
    SeekTimestamp(targetPts);
    
    return true;
}

void CaptureVideo::SeekTimestamp (long targetPts)
{    
    if (targetPts == currentPts) return;

    if (targetPts < firstPts) targetPts = firstPts;

    long previousDeltaPts = deltaPts;

//    cout << "Seek 0 : " << " going from pts " << currentPts << " to pts " << targetPts << endl;

    // seek frame
    int attempts = 0;
    long t = targetPts;
    long step = 1.0 / fps / av_q2d(video_stream->time_base) + 0.5;
    do
    {
	av_seek_frame(format_context, video_stream_idx, t, AVSEEK_FLAG_BACKWARD);
	avcodec_flush_buffers(codec_context);
	ConvertFrame();
	GrabFrame();
//	cout << "Seek 1a : is now at pts " << nextPts << " requested " << t << endl;
	t -= step;	
    }	while (nextPts > targetPts && attempts++ < 30);

//    cout << "Seek 1b : is now at pts " << nextPts << endl;

    // still ahead of desired frame ? then rewind
    if (nextPts > targetPts) 
    {
	av_seek_frame(format_context, video_stream_idx, firstPts, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
	avcodec_flush_buffers(codec_context);
	ConvertFrame();
	GrabFrame();
    }    

//    cout << "Seek 2 : is now at pts " << nextPts << endl;

//    cout << "Seek 3 : ";

    // now go forward until desired frame is reached
//    long step = 1.0 / fps / av_q2d(video_stream->time_base) + 0.5;
    while (nextPts <= targetPts)
    {
	// convert only the final 5 frames
	if (targetPts - nextPts <= step * 5) 
	    ConvertFrame();

	if (!GrabFrame())
	    break;

//	cout << nextPts << " ";
    }
//    cout << endl;

    // we jumped frames back or forward, but we try to maintain same playback
    deltaPts = previousDeltaPts;
    frameDelay.Assign (deltaPts * av_q2d(video_stream->time_base) * 1000000.0);

    frameNumber = RecalculateFrameNumberFromTimestamp();
    
//    cout << "Seek 4 : is now at pts " << nextPts << " deltaPts " << deltaPts << " frameDelay " << frameDelay.ToDouble() / 1000000.0 << " frame " << frameNumber << endl;

    calibration.Undistort(frame);

    nextFrameTime += frameDelay * playSpeed;
    statusChanged = true;
}

bool CaptureVideo::GetPreviousFrame()
{
    long step = 1.0 / fps / av_q2d(video_stream->time_base) + 0.5;
    SeekTimestamp(currentPts - step);

    return true;
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
    // calculate desired time
    double desiredTime = (double)desiredFrame / fps;

    return GetFrame(desiredTime);
}

// adapted from opencv
long CaptureVideo::GetFrameCount ()
{
    long nbf = video_stream->nb_frames;

    // did not work ? estimate from sec duration
    if (nbf == 0)
        nbf = (long)floor(GetDuration() * fps + 0.5);

    return nbf;
}

double CaptureVideo::GetDuration()
{
    double sec = (double)format_context->duration / (double)AV_TIME_BASE;

    if (sec < 0.000025)
    {
        sec = (double)video_stream->duration * av_q2d(video_stream->time_base);
    }
    return sec;
}


void CaptureVideo::SetSpeedFaster(int speed)
{
    if (speed > 1)
	playSpeed.Assign(1.0 / (double)speed); 
    else 
	playSpeed.Assign (1);

    nextFrameTime = wxGetUTCTimeUSec() + frameDelay * playSpeed;
}

void CaptureVideo::SetSpeedSlower(int speed)
{
    if (speed > 1)
	playSpeed.Assign(speed); 
    else 
	playSpeed.Assign(1);

    nextFrameTime = wxGetUTCTimeUSec() + frameDelay * playSpeed;
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
