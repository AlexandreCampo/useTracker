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

#include "SimpleTags.h"

#include "ImageProcessingEngine.h"
#include "Blob.h"
#include "Utils.h"

using namespace std;
using namespace cv;

SimpleTags::SimpleTags()
{

}

SimpleTags::~SimpleTags()
{
    CloseOutput();
}

void SimpleTags::Reset()
{
    if (pwidth == 0 || pheight == 0)
    {
	pwidth = 100;
	pheight = 20;
    }

    pattern = Mat(pheight, pwidth, CV_32S);
    p.resize(pwidth);
}

void SimpleTags::Apply()
{
    vector<Blob>& blobs = pipeline->parent->blobs;

    for (unsigned int b = 0; b < blobs.size(); b++)
    {
	blobs[b].tagId = -1;

	if (!blobs[b].available) continue;

	pattern.setTo(255*3);

	// detected blob position 
	int minx = pwidth - 1;
	int maxx = 0;

	// min, max, and pixel mean intensity
	int meanp = 0;
	int minp = 255 * 3;
	int maxp = 0;
	
	// find the rotated and close shot of the blob
	for (int y = 0; y < pheight; y++)
	{
	    for (int x = 0; x < pwidth; x++)
	    {
		// apply rotation
		float px = (float)x - pwidth / 2.0;
		float py = (float)y - pheight / 2.0;
		float d = sqrt(px*px + py*py);

		// apply scaling (take X% more pic to have it all)
		d *= blobs[b].length / (float)pwidth * 1.03;

		// get transformed coordinates
		float a = atan2 (py, px) + blobs[b].angle;
		int fx = d * cos (a) + blobs[b].x;
		int fy = d * sin (a) + blobs[b].y;

		// stay within bounds of frame
		if (fy >= 0 && fy < pipeline->height && fx >= 0 && fx < pipeline->width)
		{
		    // check if frame pixel is part of a blob
		    int idx = pipeline->labels.at<int>(fy,fx);
		    
		    if (idx >= 0)
		    {
			unsigned int label = blobs[idx].assignment;

			if (label == b)
			{
			    // calculate min/max of pattern
			    if (x < minx) minx = x;
			    if (x > maxx) maxx = x;
			    
			    // project frame pixel into pattern
			    Vec3b bgr = pipeline->frame.at<Vec3b>(fy,fx);			
			    int v = (int)bgr[0] + (int)bgr[1] + (int)bgr[2];		       
			    pattern.at<int>(y,x) = v;
			    
			    if (v > maxp) maxp = v;
			    if (v < minp) minp = v;
			}
		    }
		}
	    }
	}
	meanp = (minp + maxp) / 2;

	// check that detected pattern is symmetrical
	float diff = 0;
	for (int x = minx; x <= maxx; x++)
	{
	    for (int y = 0; y < pheight / 2; y++)
	    {
		int v1 = pattern.at<int>(y, x);
		int v2 = pattern.at<int>(pheight - y, x);
		diff += abs (v2 - v1);
	    }
	}
	diff /= float (pheight / 2) * float (maxx - minx + 1);

	std::cout << "Symmetry estimator " << diff << std::endl;

	// now stack vertical pixels
	for (int x = 0; x < pwidth; x++) p[x] = 0;
	
	for (int y = 0; y < pheight; y++)
	{
	    for (int x = minx; x <= maxx; x++)
	    {
		int v = pattern.at<int>(y, x);
		
		if (v <= meanp) 
		    p[x]++;
	    }
	}

	// get the code
	int code = 0;
	float inc = (maxx - minx) / 6.0;
	int sum = 0;
	float nextpos = inc + minx;
	int bit = 0;
	for (int x = minx; x < pwidth; x++)
	{
	    sum += p[x];

	    // done scanning 1 bit
	    if (x >= nextpos)
	    {
		code = code << 1;

		// check bit setting, correct excess of 1's due to central axis
		if (sum > 0.5 * inc * pheight) 
		    code |= 1;

		bit++;
		if (bit == 6) break;

		sum = 0;
		nextpos += inc;
	    }
	}

	// flip code ?
	if ( (code & 0x01) && ((code & 0x20) == 0) )
	{
	    int tmp = code;
	    code = 0;
	    for (int c = 0; c < 6; c++)
	    {
		code = code << 1;
		code |= tmp & 0x01; 
		tmp = tmp >> 1;
	    }
	}
	
	// check code has start / stop bits
	if ( ((code & 0x01) == 0) && (code & 0x20) )
	{
	    code = (code & ~0x20) >> 1;
	    blobs[b].tagId = code;
	}
    }
}


void SimpleTags::OutputHud(Mat& hud)
{
    char str[32];
    Point pos;
    Point pos2;
    vector<Blob>& blobs = pipeline->parent->blobs;
    for (vector<Blob>::iterator b = blobs.begin(); b != blobs.end(); ++b)
    {
	if (b->available && (b->tagId >= 0))
	{
	    sprintf (str, "id=%d", b->tagId);
	    int sqlen = sqrt(b->size) / 2;
	    pos.x = b->x;
	    pos.y = b->y;
	    pos2.x = b->x + cos(b->angle) * sqlen;
	    pos2.y = b->y + sin(b->angle) * sqlen;

	    line(hud, pos, pos2, cvScalar(255, 0, 127,255), 1);
	    putText(hud, str, pos+Point(4,4), FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,0,0,255), 2, CV_AA);
	    putText(hud, str, pos, FONT_HERSHEY_SIMPLEX, 0.65, cvScalar(0,255,200,255), 2, CV_AA);
	}
    }

    // DEBUG stuff
    // display last detected pattern
    double minv, maxv;
    minMaxIdx(pattern, &minv, &maxv);
    Mat pattern2;
    pattern.convertTo (pattern2, CV_8U, 255.0 / maxv);


    for (int y = 0; y < pheight; y++)
    {
	for (int x = 0; x <= pwidth; x++)
	{
	    int v = pattern.at<int>(y, x);
	    Vec3b rgb;
	    rgb[0] = v;
	    rgb[1] = v;
	    rgb[2] = v;

	    hud.at<Vec3b>(y + 50, x + 50) = rgb;
	}
    }


    // Mat pattern3;
    // cvtColor (pattern2, pattern3, CV_GRAY2BGR);
    // pattern3.copyTo(hud(Rect(50, 50, pattern3.cols, pattern3.rows)));



}

void SimpleTags::LoadXML (cv::FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	pwidth = (int)fn["TagWidth"];
	pheight = (int)fn["TagHeight"];
    }
}

void SimpleTags::SaveXML (cv::FileStorage& fs)
{
    fs << "Active" << active;
    fs << "TagWidth" << pwidth;
    fs << "TagHeight" << pheight;
}

void SimpleTags::OutputStep()
{
    // in any case, also output blob list with characs...
    if (outputStream.is_open())
    {
	for (unsigned int b = 0; b < pipeline->parent->blobs.size(); b++)
	{
	    outputStream
		<< pipeline->parent->capture->GetTime() << "\t"
		<< pipeline->parent->capture->GetFrameNumber() << "\t"
		<< pipeline->parent->blobs[b].tagId << "\t"
		<< pipeline->parent->blobs[b].x << "\t"
		<< pipeline->parent->blobs[b].y << "\t"
		<< pipeline->parent->blobs[b].angle << "\t"
		<< pipeline->parent->blobs[b].size << "\t"
		<< pipeline->parent->blobs[b].zone
		<< std::endl;
	}
    }
}

void SimpleTags::CloseOutput()
{
    if (outputStream.is_open()) outputStream.close();
}

void SimpleTags::OpenOutput()
{
    // open normal output
    if (outputStream.is_open()) outputStream.close();

    if (!outputFilename.empty())
    {
	outputStream.open(outputFilename.c_str(), std::ios::out);
	if (outputStream.is_open())
	{
	    outputStream << "time \t frame \t id \t x \t y \t angle \t size \t zone" << std::endl;
	}
    }
}

void SimpleTags::SetTagDimensions (int width, int height)
{
    pwidth = width;
    pheight = height;

    Reset();
}
