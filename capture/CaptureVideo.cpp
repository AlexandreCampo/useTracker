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
    av_dump_format(format_context, 0, filename.c_str(), 0);
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
    avpicture_fill((AVPicture *)&frameBGR, frame.data, PIX_FMT_BGR24, width, height);
    
//    Rewind();
    
    GrabFrame();
    ConvertFrame();

//    firstFrameNumber = frameNumber;
    // long pts;

    // if (avpacket.pts != AV_NOPTS_VALUE && avpacket.pts != 0)
    // 	pts = avpacket.pts;
    // else
    // 	pts = avpacket.dts;

    firstPts = nextPts;
    cout << "Init : first pts = " << firstPts << "  nextPts = " << nextPts << " start time = " << video_stream->start_time << endl;
    
    frameNumber = 0;

    playSpeed.Assign(1.0);
//    playTimestep.Assign(1000000.0 / fps);
    isPaused = true;

    cout << "detected w/h/fps " << width << " " << height << " " << fps << std::endl;

    cout << "total frames = " << GetFrameCount() << " duration = " << GetDuration() << " est fps = " << (double)(GetFrameCount())/GetDuration() << endl;

//    cout << "play timestep = " << playTimestep.ToDouble() << endl;

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
		
//		cout << "grab : pkt pts/dts = " << avpacket.pts << ":" << avpacket.dts << " f pts/dts = " << avframe->pkt_pts << ":" << avframe->pkt_dts << " firstPTS = " << firstPts << " frameNumber = " << frameNumber << " time = " << GetTime() << " nextT = " << nextFrameTime.ToDouble() / 1000000.0 << " video_tb = " << video_stream->time_base.num << ":" << video_stream->time_base.den  << " vcodec_tb = " << video_stream->codec->time_base.num << ":" << video_stream->codec->time_base.den << endl; 
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

    // TODO
//    frameBGR.data[0] = (uint8_t*)realloc(frameBGR.data[0], avpicture_get_size( AV_PIX_FMT_BGR24, width, height));
//    frameBGR.data[0] = (uint8_t *)frame.data;
//    avpicture_fill( (AVPicture*)&frameBGR, frameBGR.data[0], AV_PIX_FMT_BGR24, width, height );
    avpicture_fill( (AVPicture*)&frameBGR, frame.data, AV_PIX_FMT_BGR24, width, height );


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
//    memcpy (frame.data, frameBGR.data[0], frame.step[0] * height);

//    frame.data = frameBGR->data[0];
//    frame.step[0] = frameBGR->linesize[0];
//    frame = Mat (height, width, CV_8UC3, frameBGR->data[0], frameBGR->linesize[0]);

    return true;
}

// A new frame has been grabbed, calculate next pts
void CaptureVideo::EstimateFrameTimings()
{
    // back up previous value
//    currentPts = nextPts;
//    lastPts = currentPts;
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
//	nextPts = currentPts + delay;
    }

    deltaPts = nextPts - currentPts;
    frameDelay.Assign (deltaPts * av_q2d(video_stream->time_base) * 1000000.0);
//    nextPts = (nextPts - firstPts) * av_q2d(video_stream->time_base);

	// double delay = av_q2d(video_stream->codec->time_base);
	// delay += avframe->repeat_pict * (delay * 0.5); // MPEG2, frame can be repeated

//    cout << "Timings : pts = " << pts << " lastPts = " << lastPts << endl;
    
    // check frame delay for unusual values
    // double delay = nextPts - currentPts;

    // if (delay <= 0.0 || delay >= 10.0) 
    // {
    //     delay = currentFrameDelay;
    // 	nextPts = currentPts + delay;
    // }
    // else 
    // {
    //     currentFrameDelay = delay;
    // }

    cout << "Timings : nextPts = " << nextPts << " nextPts = " << nextPts << "currentPts = " << currentPts << " delay = " << deltaPts << " frameDelay = " << frameDelay.ToDouble() / 1000000.0 << endl;
}

bool CaptureVideo::GetNextFrame ()
{
    frameNumber = RecalculateFrameNumberFromTimestamp();
    ConvertFrame();

    if (!GrabFrame ())
	return false;
    
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

	cout << "Pressed PLAY : nextFrameTime = " << nextFrameTime.ToDouble() / 1000000.0 << " frameDelay  = " << frameDelay.ToDouble() << " playSpeed = " << playSpeed.ToDouble() << endl;
//	nextFrameTime = wxGetUTCTimeUSec() + playTimestep;

	isPaused = false;
	isStopped = false;
	statusChanged = true;
    }
}

long CaptureVideo::RecalculateFrameNumberFromTimestamp()
{
    // long pts;

    // if (avpacket.pts != AV_NOPTS_VALUE && avpacket.pts != 0)
    // 	pts = avpacket.pts;
    // else 
    // 	pts = avpacket.dts;

    double time = (nextPts - firstPts) * av_q2d(video_stream->time_base);

    // long int ts = av_frame_get_best_effort_timestamp(frame);
    // double time = double(ts - video_stream->start_time) / fpsi;

    frameNumber = (long)(fps * time + 0.5);

    // TODO DEBUG
//    cout << "Recalculate from timestamp : pts = " << currentPts << "pkt= " << avpacket.pts << ":" <<avpacket.dts << " start time = " << video_stream->start_time << " frame=" << frameNumber << " time = " << time << endl;

    return frameNumber;
}

long CaptureVideo::GetFrameNumber ()
{
    // frameNumber = (long)(fps * GetTime() + 0.5);

    // TODO DEBUG
//    cout << "Get frame number = " << frameNumber << endl;

    return RecalculateFrameNumberFromTimestamp();
}

double CaptureVideo::GetTime ()
{
//     long pts;

//     if (avpacket.pts != AV_NOPTS_VALUE && avpacket.pts != 0)
// 	pts = avpacket.pts;
//     else
// 	pts = avpacket.dts;
    
// //    pts -= firstFrameNumber;

     double time = (double)(currentPts - firstPts) * av_q2d(video_stream->time_base);

//    double time = (double) frameNumber / fps;

    // TODO DEBUG
//    cout << "Get time : pts = " << pts << "pkt= " << avpacket.pts << ":" <<avpacket.dts << " start time = " << video_stream->start_time << " time = " << time << endl;

    // long int ts = av_frame_get_best_effort_timestamp(frame);
    // double time = double(ts - video_stream->start_time) / fpsi;
    return time;
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
//    GetFrameNumber();
    frameNumber = 0;
}


bool CaptureVideo::GetFrame (double desiredTime)
{       
    // get current frame    
    long targetPts = firstPts + desiredTime / av_q2d(video_stream->time_base);

    SeekTimestamp(targetPts);
    
    return true;
}

void CaptureVideo::SeekTimestamp (long targetPts)
{    
    if (targetPts == currentPts) return;

    if (targetPts < firstPts) targetPts = firstPts;

    long previousDeltaPts = deltaPts;
						       
    cout << "Seek 0 : " << " going from pts " << currentPts << " to pts " << targetPts << endl;
        
    // first try to use seek, if goal is far away
//    if (abs(desiredFrame - frameNumber) > fps * 10)
//    for (int i = 0; i < 4; i++)
    //  {
//    int flags = AVSEEK_FLAG_BACKWARD;    
    // if (targetPts < currentPts)
    // 	flags = AVSEEK_FLAG_BACKWARD;
    // else
    // 	flags = AVSEEK_FLAG_ANY;

    // if (frameNumber > desiredFrame) 
    //     flags |= AVSEEK_FLAG_BACKWARD;
    
//	int flags = AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY;
    
//	long targetPTS = firstPts + long(desiredFrame / fps / av_q2d(video_stream->time_base) + 0.5);
//	cout << "Seek 1 : asking for PTS " << targetPTS << endl;


    // search 3 frames backwards, as it is common have one or 2 missing frames
//    long step = 1.0 / fps / av_q2d(video_stream->time_base) + 0.5;
    av_seek_frame(format_context, video_stream_idx, targetPts, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(codec_context);
    ConvertFrame();
    GrabFrame();

    // int attempts = 0;
    // long ts = targetPts;
    // long step = 1.0 / fps / av_q2d(video_stream->time_base) + 0.5;
    // if (step < 1) step = 1;

    // do 
    // {
    // 	ts -= step;
    // 	av_seek_frame(format_context, video_stream_idx, ts, flags);
    // 	avcodec_flush_buffers(codec_context);
    // 	GrabFrame();
    // 	cout << "Seek 2 : is now at pts " << nextPts << endl;
    // // }
    // 	attempts++;
    // } while (nextPts > targetPts && attempts < 30);

    // still ahead of desired frame ? 
    if (nextPts > targetPts) 
    {
	av_seek_frame(format_context, video_stream_idx, firstPts, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
	avcodec_flush_buffers(codec_context);
	ConvertFrame();
	GrabFrame();
    }
    cout << "Seek 2.5 : is now at pts " << nextPts << endl;
    
    
    // now go forward until desired frame is reached
//    long = frameNumber;

    long step = 1.0 / fps / av_q2d(video_stream->time_base) + 0.5;

    while (nextPts <= targetPts)
//    for (int i = 0; i < desiredFrame - frameNumber; i++) 
    {
	// convert only the final 5 frames
	if (targetPts - nextPts <= step * 5) 
	    ConvertFrame();
	if (!GrabFrame())
	    break;
	// if (GetFrameTimestamp() >= desiredFrame) break; // tried hard to match stuff with mariano...
	cout << "Seek 3 : is now at pts " << nextPts << endl;
//	currentFrame++;
    }

    // currentPts = nextPts;
    // lastPts = nextPts;

    // we jumped frames back or forward, but we try to maintain same playback
    deltaPts = previousDeltaPts;
    frameDelay.Assign (deltaPts * av_q2d(video_stream->time_base) * 1000000.0);


//	while (GetFrameTimestamp() < desiredFrame) GetNextFrame();
    frameNumber = RecalculateFrameNumberFromTimestamp();
    cout << "Seek 4 : is now at frame " << frameNumber << endl;
   
//    source.set(CV_CAP_PROP_POS_MSEC, time * 1000.0);

//    ConvertFrame();
//    GrabFrame();

//    GetFrameNumber();

    // Mat previousFrame = frame;
    // source >> frame;
    calibration.Undistort(frame);

    nextFrameTime += frameDelay * playSpeed;
//    nextFrameTime += playTimestep;
    statusChanged = true;

    // TODO always true ? 
//    return true;
}

bool CaptureVideo::GetPreviousFrame()
{
    long step = 1.0 / fps / av_q2d(video_stream->time_base) + 0.5;
    SeekTimestamp(currentPts - step);

    // long cpts = currentPts;
    // long target = cpts;
    // int attempts = 0;
    // while (attempts < 10 && cpts == currentPts)
    // {
    // 	cout << "BW SEEK TRY ----------" << endl;
    // 	target -= step;
    // 	SeekTimestamp(target);
    // 	attempts++;
    // }

//    GetFrame (GetTime() - 1.01 * (frameDelay.ToDouble() / 1000000.0));
    // int f = GetFrameNumber();
    // bool ok = GetFrame(f-1);

//    nextFrameTime += playTimestep;
//    nextFrameTime += frameDelay * playSpeed;
//    return true;
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
    {
//	cout << "Estimating total number of frames from duration and fps" << endl;
	double sec = GetDuration();
		
        nbf = (long)floor(sec * fps + 0.5);
    }
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
//    nextFrameTime = wxGetUTCTimeUSec() + playTimestep;
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
