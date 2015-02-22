
#include "SimpleTags.h"

// (Blob& blob, Mat& frame, int pwidth, int pheight, float minBlobSize, int videoWidth, int videoHeight, Mat& inMotion, int verbose, char* filename)

void SimpleTags::SimpleTags()
{

}

void SimpleTags::Reset()
{

}

void SimpleTags::OutputHud()
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

	Mat pattern(pheight, pwidth, CV_8UC1);
	Mat flip(pheight, pwidth, CV_8UC1);

	for (int y = 0; y < pheight; y++)
	{
	    for (int x = 0; x < pwidth; x++)
	    {
		// apply rotation
		float px = (float)x - pwidth / 2.0;
		float py = (float)y - pheight / 2.0;
		float d = sqrt(px*px + py*py);
		
		float a = atan2 (py, px) + blob.angle;
//	    a = atan2 (py, px);
		int fx = d * cos (a) + blob.x;
		int fy = d * sin (a) + blob.y;
		
		// stay within bounds of frame
		if (fy >=0 && fy < pipeline->height && fx >= 0 && fx < pipeline->width)
		{
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
		else
		{
		    pattern.at<unsigned char>(y,x) = 0;
//		pattern[x+y*pwidth] = 0;
		}
	    }
	}
//    for (int x = 0; x < pwidth; x++) pattern [x] = 0;
	
	// now stack vertical pixels
	int p[500];
	for (int x = 0; x < pwidth; x++) p[x] = 0;
	for (int y = 0; y < pheight; y++)
	{
	    for (int x = 0; x < pwidth; x++)
	    {
		p[x] += pattern.at<unsigned char>(y, x);
	    }
	}
	// equalize + find mean val
	int min = pwidth * 255;
	int max = 0;
	for (int x = 0; x < pwidth; x++)
	{
	    if (p[x] > max) max = p[x];
	    if (p[x] < min) min = p[x];
	}
	int mean = (min + max) / 2;
	
	// todo maybe median is better here...
	
	// threshold
	for (int x = 0; x < pwidth; x++)
	    if (p[x] >= mean) p[x] = 255; else p[x] = 0;
	
	// discriminate front / rear
	int left = 0;
	int right = 0;
	for (int x = 0; x < pwidth / 2; x++)
	    left += p[x];
	for (int x = pwidth / 2; x < pwidth; x++)
	    right += p[x];
	
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
	SaveMatToPNG(pattern, "pattern.png");

    // finally stack the vertical pixels and output stats, and apply threshold

    // then decide what is the tag id...
    }

//    delete [] pattern;
//    delete [] flip;
}
