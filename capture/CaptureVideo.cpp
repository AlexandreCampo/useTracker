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

    avpacket = av_packet_alloc();

    av_log_set_level(AV_LOG_WARNING);

    // Open video file
    int err = avformat_open_input(&format_context, filename.c_str(), NULL, NULL);
    if (err < 0)
    {
	char errbuf[AV_ERROR_MAX_STRING_SIZE];
	av_strerror(err, errbuf, sizeof(errbuf));
	cerr << "Error : Could not open video file " << filename << " (" << errbuf << ")" << endl;
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
    codec = avcodec_find_decoder(video_stream->codecpar->codec_id);
    if (!codec)
    {
	cerr << "Error : Failed to find codec" << endl;
	return 0;
    }

    codec_context = avcodec_alloc_context3(codec);
    if (!codec_context)
    {
	cerr << "Error : Failed to allocate codec context" << endl;
	return 0;
    }

    if (avcodec_parameters_to_context(codec_context, video_stream->codecpar) < 0)
    {
	cerr << "Error : Failed to copy codec parameters to context" << endl;
	return 0;
    }

    if ((ret = avcodec_open2(codec_context, codec, NULL)) < 0)
    {
	cerr << "Error : Failed to open codec" << endl;
	return 0;
    }

    avframe = av_frame_alloc();

    if (video_stream->avg_frame_rate.den > 0)
	fps = av_q2d(video_stream->avg_frame_rate);
    else
	fps = av_q2d(video_stream->r_frame_rate);

    // Determine required buffer size and allocate buffer
    width = codec_context->width;
    height = codec_context->height;

    cout << "detected w/h/fps " << width << " " << height << " " << fps << std::endl;

    switch (codec_context->pix_fmt)
    {
    case AV_PIX_FMT_YUVJ420P : pixel_format = AV_PIX_FMT_YUV420P; break;
    case AV_PIX_FMT_YUVJ422P : pixel_format = AV_PIX_FMT_YUV422P; break;
    case AV_PIX_FMT_YUVJ444P : pixel_format = AV_PIX_FMT_YUV444P; break;
    case AV_PIX_FMT_YUVJ440P : pixel_format = AV_PIX_FMT_YUV440P; break;
    default:
        pixel_format = codec_context->pix_fmt;
        break;
    }

    // prepare context for conversion to opencv Mat
    img_convert_ctx = sws_getContext(width, height,
				     pixel_format,
				     width, height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR,
				     NULL, NULL, NULL);

    // Assign opencv mat buffer to image planes in frameBGR
    frame = Mat::zeros (height, width, CV_8UC3);
    frameBGR = av_frame_alloc();
    av_image_fill_arrays(frameBGR->data, frameBGR->linesize,
                         frame.data, AV_PIX_FMT_BGR24, width, height, 1);

    GrabFrame();
    ConvertFrame();
    calibration.Undistort(frame);

    deltaPts = (int64_t)(1.0 / fps / av_q2d(video_stream->time_base) + 0.5); // initial approximation
    frameDelay = (int64_t)(deltaPts * av_q2d(video_stream->time_base) * 1000000.0);
    currentPts = nextPts;
    firstPts = nextPts;
    frameNumber = 0;
    playSpeedMul = 1; playSpeedDiv = 1;
    isPaused = true;
    nextFrameTime = GetUTCTimeUSec();

    return true;
}

void CaptureVideo::Close ()
{
    // Free the frames
    if (avframe) av_frame_free(&avframe);
    if (frameBGR) av_frame_free(&frameBGR);

    // Free the packet
    if (avpacket) av_packet_free(&avpacket);

    // Close the codec context
    if (codec_context) avcodec_free_context(&codec_context);

    // close convert context
    if (img_convert_ctx) sws_freeContext(img_convert_ctx);
    img_convert_ctx = nullptr;

    // Close the video file
    if (format_context) avformat_close_input(&format_context);
}

bool CaptureVideo::GrabFrame ()
{
    while (1)
    {
	// get next packet
	av_packet_unref(avpacket);
	int ret = av_read_frame(format_context, avpacket);

	if (ret < 0) break;
        if (ret == AVERROR(EAGAIN)) continue;

	// Is this a packet from the video stream?
	if(avpacket->stream_index == video_stream_idx)
	{
	    // Send packet to decoder
	    ret = avcodec_send_packet(codec_context, avpacket);
	    if (ret < 0) break;

	    // Receive decoded frame
	    ret = avcodec_receive_frame(codec_context, avframe);
	    if (ret == AVERROR(EAGAIN)) continue;
	    if (ret < 0) break;

	    EstimateFrameTimings();
	    return true;
	}
    }
    return false;
}

bool CaptureVideo::ConvertFrame ()
{
    // prepare context for conversion to opencv Mat
    if (img_convert_ctx == NULL)
	img_convert_ctx = sws_getContext(width, height,
					 pixel_format,
					 width, height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR,
					 NULL, NULL, NULL);

    av_image_fill_arrays(frameBGR->data, frameBGR->linesize,
                         frame.data, AV_PIX_FMT_BGR24, width, height, 1);

    // Convert the image from its native format to BGR opencv Mat
    sws_scale(img_convert_ctx, avframe->data, avframe->linesize, 0, height, frameBGR->data, frameBGR->linesize);

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
    else if (avframe->pts != AV_NOPTS_VALUE)
    {
	nextPts = avframe->pts;
    }
    // no valid pts found, make a prediction instead
    else
    {
	nextPts = currentPts + deltaPts;
    }

    deltaPts = nextPts - currentPts;
    frameDelay = (int64_t)(deltaPts * av_q2d(video_stream->time_base) * 1000000.0);
}

bool CaptureVideo::GetNextFrame ()
{
    frameNumber = RecalculateFrameNumberFromTimestamp();
    ConvertFrame();

    if (!GrabFrame ())
	return false;

    calibration.Undistort(frame);

    nextFrameTime += frameDelay * playSpeedMul / playSpeedDiv;

    return true;
}

int64_t CaptureVideo::GetNextFrameSystemTime()
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
	nextFrameTime = GetUTCTimeUSec() + frameDelay * playSpeedMul / playSpeedDiv;

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
    frameDelay = (int64_t)(deltaPts * av_q2d(video_stream->time_base) * 1000000.0);

    ConvertFrame();
    frameNumber = 0;
}


bool CaptureVideo::GetFrame (double desiredTime)
{
    long targetPts = firstPts + desiredTime / av_q2d(video_stream->time_base);
    SeekTimestamp(targetPts);

    return true;
}

void CaptureVideo::SetTime (double desiredTime)
{
    GetFrame(desiredTime);
}

void CaptureVideo::SeekTimestamp (long targetPts)
{
    if (targetPts == currentPts) return;

    if (targetPts < firstPts) targetPts = firstPts;

    long previousDeltaPts = deltaPts;

    // get closer from target frame, seeking to non keyframes
    av_seek_frame(format_context, video_stream_idx, targetPts, AVSEEK_FLAG_ANY);
    avcodec_flush_buffers(codec_context);
    ConvertFrame();
    GrabFrame();

    // seek frame
    int attempts = 0;
    long t = targetPts;
    long step = (long)(1.0 / fps / av_q2d(video_stream->time_base) + 0.5);
    do
    {
	av_seek_frame(format_context, video_stream_idx, t, AVSEEK_FLAG_BACKWARD);
	avcodec_flush_buffers(codec_context);
	ConvertFrame();
	GrabFrame();
	t -= step;
    }	while (nextPts > targetPts && attempts++ < 30);

    // still ahead of desired frame ? then rewind
    if (nextPts > targetPts)
    {
	av_seek_frame(format_context, video_stream_idx, firstPts, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY);
	avcodec_flush_buffers(codec_context);
	ConvertFrame();
	GrabFrame();
    }

    // now go forward until desired frame is reached
    while (nextPts <= targetPts)
    {
	// convert only the final 5 frames
	if (targetPts - nextPts <= step * 5)
	    ConvertFrame();

	if (!GrabFrame())
	    break;
    }

    // we jumped frames back or forward, but we try to maintain same playback
    deltaPts = previousDeltaPts;
    frameDelay = (int64_t)(deltaPts * av_q2d(video_stream->time_base) * 1000000.0);

    frameNumber = RecalculateFrameNumberFromTimestamp();

    calibration.Undistort(frame);

    nextFrameTime += frameDelay * playSpeedMul / playSpeedDiv;
    statusChanged = true;
}

bool CaptureVideo::GetPreviousFrame()
{
    long step = (long)(1.0 / fps / av_q2d(video_stream->time_base) + 0.5);
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
    playSpeedMul = 1;
    playSpeedDiv = (speed > 1) ? speed : 1;
    nextFrameTime = GetUTCTimeUSec() + frameDelay * playSpeedMul / playSpeedDiv;
}

void CaptureVideo::SetSpeedSlower(int speed)
{
    playSpeedMul = (speed > 1) ? speed : 1;
    playSpeedDiv = 1;
    nextFrameTime = GetUTCTimeUSec() + frameDelay * playSpeedMul / playSpeedDiv;
}

void CaptureVideo::SetPlaySpeed(int level)
{
    // level > 0: faster (2^level x), level < 0: slower (2^|level| x), 0: normal
    if (level > 0) {
        playSpeedMul = 1;
        playSpeedDiv = 1 << level;
    } else if (level < 0) {
        playSpeedMul = 1 << (-level);
        playSpeedDiv = 1;
    } else {
        playSpeedMul = 1;
        playSpeedDiv = 1;
    }
    nextFrameTime = GetUTCTimeUSec() + frameDelay * playSpeedMul / playSpeedDiv;
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
