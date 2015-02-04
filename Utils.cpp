
#include "Utils.h"

#include <vector>
#include <stack>
#include <iostream>


using namespace std;


void EqualizeToRGB (Mat& src, Mat& dst)
{
    unsigned char min = 255, max = 0;

    for (int i = 0; i < src.rows * src.cols; i++)
    {
	unsigned char v = src.data[i];
	if (v < min) min = v;
	if (v > max) max = v;
    }

    for (int y = 0; y < src.rows; y++)
    {
	unsigned char* row = src.ptr<unsigned char>(y);
	unsigned char* row2 = dst.ptr<unsigned char>(y);
	for (int x = 0; x < src.cols; x++)
	{
	    float v = row[x];
	    v = (v - min) / (max - min) * 255.0;
	    row2[x*3] = (unsigned char)(v);
	    row2[x*3+1] = (unsigned char)(v);
	    row2[x*3+2] = (unsigned char)(v);
	}
    }
}

void EqualizeBW (Mat& src, Mat& dst)
{
    unsigned char min = 255, max = 0;

    for (int i = 0; i < src.rows * src.cols; i++)
    {
	unsigned char v = src.data[i];
	if (v < min) min = v;
	if (v > max) max = v;
    }

    for (int y = 0; y < src.rows; y++)
    {
	unsigned char* row = src.ptr<unsigned char>(y);
	for (int x = 0; x < src.cols; x++)
	{
	    float v = row[x];
	    v = (v - min) / (max - min) * 255.0;
	    row[x] = (unsigned char)(v);
	}
    }
}


void SaveMatToPNG (Mat& mat, const char* filename)
{
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    imwrite(filename, mat, compression_params);
}


