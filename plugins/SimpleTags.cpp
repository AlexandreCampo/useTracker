
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

	Mat pattern(pheight, pwidth, CV_8UC1);
	Mat flip(pheight, pwidth, CV_8UC1);
	pattern.setTo(255);

	int minx = pwidth - 1;
	int maxx = 0;

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

		float a = atan2 (py, px) + blobs[b].angle;
//	    a = atan2 (py, px);
		int fx = d * cos (a) + blobs[b].x;
		int fy = d * sin (a) + blobs[b].y;

		// stay within bounds of frame
		if (fy >=0 && fy < pipeline->height && fx >= 0 && fx < pipeline->width)
		{
		    if (pipeline->labels.at<int>(fy,fx) == (int)b)
		    {
			// calculate min/max of pattern
			if (x < minx) minx = x;
			if (x > maxx) maxx = x;

			// project frame pixel into pattern
			Vec3b bgr = pipeline->frame.at<Vec3b>(fy,fx);			
			int v = (int)bgr[0] + (int)bgr[1] + (int)bgr[2];
			pattern.at<unsigned char>(y,x) = (unsigned char)(v/3);

		    // if that pixel is part of inMotion map, make it white
//		bool inMotion = false;
		    // int idx = fx + fy * videoWidth;
		    // if (inMotion[idx] != 0) pattern[x+y*pwidth] = 255;
		    // for (int j = 0; j < blob.vertices.size(); j++)
		    // {
		    //     cout << "checking blob vertices " << blob.vertices[j].x << " " << blob.vertices[j].y << " " << blob.vertices[j].index << " against " << fx << " " << fy << " " << idx << endl;
		    //     if (idx == blob.vertices[j].index)
		    //     {
		    // 	inMotion = true;
		    // 	break;
		    //     }
		    // }
		    }
		}
		else
		{
//		    pattern.at<unsigned char>(y,x) = 0;
//		pattern[x+y*pwidth] = 0;
		}
	    }
	}
//    for (int x = 0; x < pwidth; x++) pattern [x] = 0;

	// now stack vertical pixels
	int p[500];
	int minp = 255;
	int max = 0;
	for (int x = 0; x < pwidth; x++) p[x] = 0;
	for (int y = 0; y < pheight; y++)
	{
	    for (int x = minx; x <= maxx; x++)
	    {
		unsigned char grey = pattern.at<unsigned char>(y, x);
		p[x] += grey;
		if (maxp < grey) maxp = grey; 
		if (minp > grey) minp = grey; 
	    }
	}

	// find mean val
	// int min = pwidth * 255;
	// int max = 0;
	// for (int x = minx; x <= maxx; x++)
	// {
	//     if (p[x] > max) max = p[x];
	//     if (p[x] < min) min = p[x];
	// }
	int mean = (minp + maxp) / 2;

	// threshold
//	for (int x = minx; x <= maxx; x++)
//	    if (p[x] >= mean) p[x] = 255; else p[x] = 0;

	// get the code
	int code = 0;
	float inc = maxx - minx;
	inc /= 6;

	std::cout << "Blob " << b << " detected code ";

	for (int c = 0; c < 6; c++)
	{
	    code = code << 1;
	    int sum = 0;
	    for (int x = minx + c * inc; x < minx + c * inc + inc; x++)		
		sum += p[x];

	    if (sum <= pheight * inc * 

	    if (sum <= inc * 255 / 2)
	    {
		code |= 1;
		std::cout << "1";
	    }
	    else
	    {
		std::cout << "0";
	    }
	}

	// flip code ?
	if ( ((code & 0x01) == 0) && (code & 0x20) )
	{
	    int tmp = code;
	    code = 0;
	    for (int c = 0; c < 6; c++)
	    {
		code |= tmp & 0x01; 
		code = code << 1;
		tmp = tmp >> 1;
	    }
	    std::cout << " flipped ";
	}
	
	// check code has start / stop bits
	if ( (code & 0x01) && ((code & 0x20) == 0) )
	{
	    code = code >> 1;
	}
	
	std::cout << " id = " << code << std::endl;

	
	blobs[b].tagId = code;


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
	for (int x = 0; x < pwidth; x++) pattern.at<unsigned char>(0,x) = p[x];


    // output a png of the rotated result (left to righ etc...)
	//Png* outputImage = new Png (pwidth, pheight);
	//SaveGreyscaleBufferToFile (pattern, outputImage, filename);
	char fname[128];
	sprintf(fname, "pattern_%d.png", b);
	SaveMatToPNG(pattern, fname);

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
	if (b->available && b->tagId >= 0)
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
