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


