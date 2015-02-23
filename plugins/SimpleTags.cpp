
#include "SimpleTags.h"

#include "ImageProcessingEngine.h"
#include "Blob.h"
#include "Utils.h"

using namespace std;
using namespace cv;

SimpleTags::SimpleTags()
{

}

void SimpleTags::Reset()
{
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
}

void SimpleTags::LoadXML (cv::FileNode& fn)
{
    if (!fn.empty())
    {
	active = (int)fn["Active"];
	pwidth = (int)fn["PatternWidth"];
	pheight = (int)fn["PatternHeight"];
    }
}

void SimpleTags::SaveXML (cv::FileStorage& fs)
{
    fs << "Active" << active;
    fs << "PatternWidth" << pwidth;
    fs << "PatternHeight" << pheight;
}
