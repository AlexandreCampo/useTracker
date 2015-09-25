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

// The code below comes from https://github.com/bsdnoobz/zhang-suen-thinning/blob/master/thinning.cpp
// And http://opencv-code.com/quick-tips/implementation-of-thinning-algorithm-in-opencv/
// thanks to the author

/**
 * Perform one thinning iteration.
 * Normally you wouldn't call this function directly from your code.
 *
 * Parameters:
 * 		im    Binary image with range = [0,1]
 * 		iter  0=even, 1=odd
 */
void ThinningIteration(cv::Mat& img, int iter)
{
    cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);

    int nRows = img.rows;
    int nCols = img.cols;

    if (img.isContinuous()) {
        nCols *= nRows;
        nRows = 1;
    }

    int x, y;
    uchar *pAbove;
    uchar *pCurr;
    uchar *pBelow;
    uchar *nw, *no, *ne;    // north (pAbove)
    uchar *we, *me, *ea;
    uchar *sw, *so, *se;    // south (pBelow)

    uchar *pDst;

    // initialize row pointers
    pAbove = NULL;
    pCurr  = img.ptr<uchar>(0);
    pBelow = img.ptr<uchar>(1);

    for (y = 1; y < img.rows-1; ++y) {
        // shift the rows up by one
        pAbove = pCurr;
        pCurr  = pBelow;
        pBelow = img.ptr<uchar>(y+1);

        pDst = marker.ptr<uchar>(y);

        // initialize col pointers
        no = &(pAbove[0]);
        ne = &(pAbove[1]);
        me = &(pCurr[0]);
        ea = &(pCurr[1]);
        so = &(pBelow[0]);
        se = &(pBelow[1]);

        for (x = 1; x < img.cols-1; ++x) {
            // shift col pointers left by one (scan left to right)
            nw = no;
            no = ne;
            ne = &(pAbove[x+1]);
            we = me;
            me = ea;
            ea = &(pCurr[x+1]);
            sw = so;
            so = se;
            se = &(pBelow[x+1]);

            int A  = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) + 
                     (*ea == 0 && *se == 1) + (*se == 0 && *so == 1) + 
                     (*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
                     (*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);
            int B  = *no + *ne + *ea + *se + *so + *sw + *we + *nw;
            int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we);
            int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                pDst[x] = 1;
        }
    }

    img &= ~marker;
}

/**
 * Function for thinning the given binary image
 *
 * Parameters:
 * 		src  The source image, binary with range = [0,1]
 * 		dst  The destination image
 */
void Thinning(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(src.channels() == 1);
    CV_Assert(src.depth() != sizeof(uchar));
    CV_Assert(src.rows > 3 && src.cols > 3);

    dst = src.clone();

    cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
    cv::Mat diff;

    do {
        ThinningIteration(dst, 0);
        ThinningIteration(dst, 1);
        cv::absdiff(dst, prev, diff);
        dst.copyTo(prev);
    } 
    while (cv::countNonZero(diff) > 0);
}



