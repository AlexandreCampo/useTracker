
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

}

void SimpleTags::Apply()
{
    vector<Blob>& blobs = pipeline->parent->blobs;

    for (unsigned int b = 0; b < blobs.size(); b++)
    {
    // rotate the blob, identify left/right by filtering red color
    // we need a table for that
    // unsigned char* pattern = new unsigned char [pwidth * pheight];
    // unsigned char* flip = new unsigned char [pwidth * pheight];

	blobs[b].tagId = -1;

	if (!blobs[b].available) continue;

	Mat pattern(pheight, pwidth, CV_32S);
	pattern.setTo(255*3);

	// debug
	Mat patternDBG(pheight, pwidth, CV_32S);
	patternDBG.setTo(0);

	// detected blob position 
	int minx = pwidth - 1;
	int maxx = 0;

	// min, max, and pixel mean intensity
	int meanp = 0;
	int minp = 255 * 3;
	int maxp = 0;
	
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
			    
			    patternDBG.at<int>(y,x) = v;
			    
			    if (v > maxp) maxp = v;
			    if (v < minp) minp = v;
			}
		    }
		    // else if (label >= 0)
		    // {
		    // 	cout << "Other label " << b << " != " << label << endl;
		    // }
		}
	    }
	}
	meanp = (minp + maxp) / 2;

	// now stack vertical pixels
	int p[500];
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

	// debug
	for (int x = 0; x < pwidth; x++) 
	    pattern.at<int>(0, x) = 0;
	for (int x = 0; x < pwidth; x++) 
	    pattern.at<int>(1, x) = 0;
	for (int x = 0; x < pwidth; x++) 
	    pattern.at<int>(2, x) = 0;

	for (int x = minx; x <= maxx; x++)
	    pattern.at<int>(0, x) = 255*3;
	for (int x = minx; x <= maxx; x++)
	    pattern.at<int>(1, x) = p[x] * 255 * 3 / pheight;

	// get the code
	int code = 0;
	float inc = maxx - minx;
	inc /= 6.0;

	std::cout << "Blob " << b << " detected code ";
	
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
		{
		    code |= 1;
		    std::cout << "1";
		}
		else
		{
		    std::cout << "0";
		}
		sum = 0;
		nextpos += inc;
		bit++;
		if (bit == 6) break;
	    }
	}

	// for (int c = 0; c < 6; c++)
	// {
	//     code = code << 1;
	//     int sum = 0;
	//     for (int x = minx + c * inc; x < minx + c * inc + inc; x++)		
	// 	sum += p[x];

	//     if (sum >= inc * pheight / 2)
	//     {
	// 	code |= 1;
	// 	std::cout << "1";
	//     }
	//     else
	//     {
	// 	std::cout << "0";
	//     }
	// }

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
	    std::cout << " flipped ";
	}
	
	// check code has start / stop bits
	if ( ((code & 0x01) == 0) && (code & 0x20) )
	{
	    code = (code & ~0x20) >> 1;
	    std::cout << " id = " << code << std::endl;
	    blobs[b].tagId = code;
	}
	else
	{
	    std::cout << " NO ID ... " << code << std::endl;
//	    blobs[b].tagId = -1;

	    std::cout << "inc = " << inc << " minx = " << minx << " maxx = " << maxx << endl;
	    std::cout << "Full ";
	    float npos = minx + inc;
	    int bc = 0;
	    for (int x = minx; x < pwidth; x++)
	    {
		std::cout << p[x] << " ";
		
		if (x >= npos)
		{
		    std::cout << endl;
		    npos += inc;
		    bc++;
		    if (bc == 6) break;
		}
	    }
	    std:: cout << endl;
	}
	

	


	// // discriminate front / rear
	// int left = 0;
	// int right = 0;
	// for (int x = 0; x < pwidth / 2; x++)
	//     left += p[x];
	// for (int x = pwidth / 2; x < pwidth; x++)
	//     right += p[x];

	// flip the pattern ?
// 	if (right > left)
// 	{
// 	    memcpy (flip, pattern, pwidth*pheight);
// 	    for (int y = 0; y < pheight; y++)
// 	    {
// 		for (int x = 0; x < pwidth; x++)
// 		{
// 		    pattern[x+y*pwidth] = flip [pwidth-1-x+y*pwidth];
// 		}
// 	    }
// //	memcpy (flip, p, pwidth);
// 	    for (int x = 0; x < pwidth; x++)
// 	    {
// 		pattern[x] = flip [pwidth-1-x];
// 	    }
// 	}

    // the rest needs checking with real exps...

    // save for debug
//	for (int x = 0; x < pwidth; x++) pattern.at<unsigned char>(0,x) = p[x];


    // output a png of the rotated result (left to righ etc...)
	//Png* outputImage = new Png (pwidth, pheight);
	//SaveGreyscaleBufferToFile (pattern, outputImage, filename);
	char fname[128];
	Mat output;
	sprintf(fname, "pattern_%d.png", b);
	pattern.convertTo(output, CV_8UC1);
	SaveMatToPNG(output, fname);

	sprintf(fname, "pattern_%d_dbg.png", b);
	patternDBG.convertTo(output, CV_8UC1);
	SaveMatToPNG(output, fname);

    // finally stack the vertical pixels and output stats, and apply threshold

    // then decide what is the tag id...
    }

//    delete [] pattern;
//    delete [] flip;
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

}

void SimpleTags::SaveXML (cv::FileStorage& fs)
{

}
