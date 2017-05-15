
// compile with :
// g++ `pkg-config --cflags opencv` printMarkers.cpp marker.cpp -o print -std=c++11  `pkg-config --libs opencv`

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "marker.h"

using namespace cv; 
using namespace std; 

int hues[] = {20,  30,  40,  65,  85,  95, 125, 140, 165};


int k = 10;
int borderWidth = 4 * k;
int colorWidth = 15 * k;
int colorSeparation = 3 * k;
int borderSeparation = 3 * k;

int markersPerLine = 5;

int N = 4;
int C = 9;

// get a list of markers, print them
int main(int argc, char** argv)
{
    Marker::numPixels = N;
    Marker::numHues = C;
    
    borderWidth = atoi(argv[1]);
    borderSeparation = atoi(argv[2]);
    colorWidth = atoi(argv[3]);
    colorSeparation = atoi(argv[4]);

    int markerWidth = colorWidth * 2 + colorSeparation + borderSeparation * 2 + borderWidth * 2;
    int markerSeparation = borderSeparation * 5;

    if (markerSeparation == 0) markerSeparation = colorWidth / 2;
    
    vector<Marker> dict;

    for (int i = 5; i < argc; i++)
    {
		Marker m (atoi(argv[i]));
		dict.push_back(m);
    }

    // create pic
    int sx = 100 * 2 + markerWidth * markersPerLine + markerSeparation * (markersPerLine - 1);
    int lines = dict.size() / markersPerLine;
    if (dict.size() % markersPerLine) lines++;
    int sy = 100 * 2 + markerWidth * lines + markerSeparation * (lines - 1);
    Mat img(sy, sx, CV_8UC3);

    // fill with white
    img = Scalar(0,0,255);
    
    int x = markerSeparation;
    int y = markerSeparation;

    // loop over markers and draw them
    int xcount = 0;
    for (int i = 0; i < dict.size(); ++i)
    {
		Marker m = dict[i];

		int px = x, py = y;
//		int xinc = 20, yinc = 20;
		cout << "Marker " << i << " = " << (int) m.getCode() << " code = ";
		
		// create black borders of 3px around each marker
		Scalar noir(0,0,0);
		
		int ppx = px - borderWidth - borderSeparation;
		int ppy = py - borderWidth - borderSeparation;
		rectangle(img, Point(ppx, ppy),  Point(ppx+markerWidth, ppy+borderWidth), noir, CV_FILLED);
		rectangle(img, Point(ppx, ppy),  Point(ppx+borderWidth, ppy+markerWidth), noir, CV_FILLED);

		ppx += markerWidth ;
		ppy += markerWidth;
		rectangle(img, Point(ppx, ppy),  Point(ppx-markerWidth, ppy-borderWidth), noir, CV_FILLED);
		rectangle(img, Point(ppx, ppy),  Point(ppx-borderWidth, ppy-markerWidth), noir, CV_FILLED);
		
		int xinc = colorWidth + colorSeparation;
		int linec = 0;
		for (int c = 0; c < N; ++c)
		{
			cout << (int)m.pixels[c] << " ";
			Scalar col (hues[m.pixels[c]], 255, 255);
			Point p1 (px, py);
			Point p2 (px + colorWidth, py + colorWidth);
			rectangle(img, p1, p2, col, CV_FILLED);

			++linec;
			if (linec == 2)
			{
				py += colorWidth + colorSeparation;
				xinc *= -1;
			}
			else px += xinc;
		}
		cout << endl;
		
		x += markerWidth + markerSeparation;

		++xcount;
		if (xcount % markersPerLine == 0)
		{
			x = markerSeparation;
			y += markerWidth + markerSeparation;
		}
    }

    // convert pic to BGR
    Mat img2;
    cvtColor(img, img2, CV_HSV2BGR);
    
    imwrite("markers.png", img2);
   
    return 0;
}
