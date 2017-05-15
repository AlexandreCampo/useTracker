
#include "arucoColor.h"

// #include <iostream>
// #include <vector>


using namespace cv; 
using namespace std;

namespace ac
{
    
    ArucoColor::ArucoColor()
    {
	setMarkersDimensions(2,2);
    }

    ArucoColor::~ArucoColor()
    {

    }

    void ArucoColor::setDictionary(std::vector<int> codes)
    {
	dictionary.clear();
	drawingColors.clear();
	
	RNG rng(12345);

	for (int i = 0; i < codes.size(); i++)
	{
	    dictionary.add(codes[i], i);

	    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255), 255);
	    drawingColors.push_back(color);
	}
	dictionary.update();
    }

    void ArucoColor::setMarkersDimensions (int numRows, int numCols)
    {
	// todo right now only circular markers
	Marker::cols = numCols;
	Marker::rows = numRows;
	Marker::numPixels = numCols * numRows;

	// rebuild distances table
	buildDistancesTable();

	// rebuild dictionary
	dictionary.update();	
    }

    void ArucoColor::buildDistancesTable()
    {
	// some changes such as reference hues, dictionary,
	// or marker dimensions require rebuilding data
	Marker::distances.clear();
	for (int i = 0; i < Marker::referenceHues.size(); i++)
	{
	    if (i <= Marker::referenceHues.size() / 2)
		Marker::distances.push_back(i);
	    else
		Marker::distances.push_back(Marker::referenceHues.size() - i);	
	}
    }

    
    void ArucoColor::setReferenceHues(std::vector<int> hues)
    {
	Marker::referenceHues = hues;
	Marker::numHues = hues.size();
    
	// fill the distances table
	buildDistancesTable();

	// update entire dictionary
	dictionary.update();
    }

    void ArucoColor::warp(const Mat &in, Mat &out, Size size, vector< Point > points)
    {
	// obtain the perspective transform
	Point2f pointsRes[4], pointsIn[4];
	for (int i = 0; i < 4; i++)
	    pointsIn[i] = points[i];
	pointsRes[0] = (Point2f(0, 0));
	pointsRes[1] = Point2f(size.width - 1, 0);
	pointsRes[2] = Point2f(size.width - 1, size.height - 1);
	pointsRes[3] = Point2f(0, size.height - 1);
	Mat M = getPerspectiveTransform(pointsIn, pointsRes);
	warpPerspective(in, out, M, size, cv::INTER_NEAREST);
    }

    void ArucoColor::segmentImage (const Mat& img)
    {    
	// adaptive threshold for segmentation
	cvtColor(img, grey, CV_BGR2GRAY);
	adaptiveThreshold(grey, segmented, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, adaptiveThresholdBlockSize*2+1, adaptiveThresholdConstant);

	// // use negative version for better detection
	// bitwise_not(segmented, segmented);

	// // detect colors
	// cvtColor(img, hsv, CV_BGR2HSV);
        
	// inRange(hsv, hsvMin, hsvMax, segmented2);

	// // erode & dilate colors
	// if (DILATE > 0)
	// {
	// 	dilate (segmented2, segmented3, structuringElement);	    
	// }

	// // merge detection of colors and white borders
	// segmented |= segmented3;
    }

    void ArucoColor::findRectangles(Mat& seg)
    {
	// now extract contours
	vector<vector<Point> > contours;
	findContours(seg, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
	// Find the convex hull object for each contour
	hulls.clear();
	hullCenters.clear();
	hullAreas.clear();
	for( int i = 0; i < contours.size(); i++ )
	{
	    double area = contourArea(contours[i]);

	    if (area >= minMarkerArea && area < maxMarkerArea)
	    {	    
		// convex hull produces a lot of lines
		// min area rect finds rects everywhere
		// approxPolyDP seems best method here
		vector<Point> h;
		approxPolyDP (contours[i], h, double(contours[i].size()) * approxPolyTolerance, true);
	    
		if (h.size() == 4  && isContourConvex(h))
		{		
		    // ensure that the   distace between consecutive points is large enough
		    float minDist = 1e10;
		    for (int j = 0; j < 4; j++)
		    {
			int k = (j + 1) % 4;
			float d = std::sqrt( (float) (h[j].x - h[k].x) * (h[j].x - h[k].x) + (h[j].y - h[k].y) * (h[j].y - h[k].y));
			if (d < minDist) minDist = d;
		    }
		
		    // check that distance is not very small
		    if (minDist > 10)
		    {		    
			// get the center
			Point center;
			center.x = (h[0].x + h[1].x + h[2].x + h[3].x) / 4;
			center.y = (h[0].y + h[1].y + h[2].y + h[3].y) / 4;
		    
			// before pushing... check that it does not overlap a previous one
			bool assigned = false;
			for (int j = 0; j < hulls.size(); j++)
			{
			    int dx = hullCenters[j].x - center.x;
			    int dy = hullCenters[j].y - center.y;
			    int d = dx*dx + dy*dy;
			
			    // same location
			    if (d < 16 * 16)
			    {				
				assigned = true;
				if (area < hullAreas[j])
				{
				    hulls[j] = h;
				    hullCenters[j] = center;
				    hullAreas[j] = area;
				}
			    }
			}
			if (!assigned)
			{
			    hullCenters.push_back(center);
			    hullAreas.push_back(area);
			    hulls.push_back(h);
			}
		    }
		}		
	    }
	}
    }

    bool ArucoColor::decodeMarker(const Mat& markerImage, Marker& candidate)
    {
	// decode the marker...
	// scan each subrectangle and make a stat to get color
	bool badMarker = false;
	candidate.hues.clear();
    
	int sx = 0;
	int sy = 0;
	for (int j = 0; j < 4; j++)
	{
	    // move scan area
	    if (j == 1) {sx = 16;}
	    else if (j == 2) {sy = 16;}
	    else if (j == 3) {sx = 0;}
	
	    float n = 0;
	
	    vector<int> hs;
	    for (int y = 0; y < 16; y++)
	    {
		const Vec3b* row = markerImage.ptr<Vec3b>(sy + y);
		for (int x = 0; x < 16; x++)
		{				
		    const Vec3b& pix = row [x + sx];
		
		    if (pix[1] >= saturationThreshold && pix[2] >= valueThreshold)
		    {				    
			hs.push_back(pix[0]);
		    }
		}
	    }
	
	    // find median hue
	    if (hs.size() > 2)
	    {
		std::sort(hs.begin(), hs.end());
		candidate.hues.push_back(hs[hs.size() / 2]);
	    }
	    else
	    {
		badMarker = true;
		break;
	    }
	}
	return !badMarker;
    }

    void ArucoColor::drawMarkers(Mat& img)
    {
	vector<vector<Point>> contours;
    
	int i = 0;
	for (const auto& m : detectedMarkers)
	{
	    contours.push_back(m.corners);
	    drawContours(img, contours, i, drawingColors[i], 3);
	    
	    std::ostringstream str;
	    str << m.id;			    
	    Point pt = m.corners[0];
	    cv::putText(img, str.str(), pt, CV_FONT_HERSHEY_PLAIN, 5, Scalar (0,0,250, 255), 3, CV_AA);		    
	    i++;
	}
    }

    std::vector<ArucoColor::DetectedMarker>& ArucoColor::detect (const Mat& img)
    {
	detectedMarkers.clear();
    
	segmentImage(img);
    
	findRectangles(segmented);

	// Draw contours + hull results
	Mat markerImage (32, 32, CV_8UC3);
	for( int i = 0; i < hulls.size(); i++ )
	{
	    // warp to get rectified rectangle
	    warp(img, markerImage, markerImage.size(), hulls[i]);

	    // convert marker image to hsv
	    cvtColor(markerImage, markerImage, CV_BGR2HSV);

	
	    Marker candidate;
	    if (decodeMarker(markerImage, candidate))
	    {	    
		// find best marker(s)
		int dist;
		int id = dictionary.findBestMarker(candidate.hues, maxHueDeviation, maxMarkerRange, dist);
	    
		if (id >= 0)
		{
		    // record detected marker
		    detectedMarkers.push_back(DetectedMarker(id, hulls[i], hullCenters[i], hullAreas[i]));
		}				    
	    }
	}
	return detectedMarkers;
    }


    void ArucoColor::setMarkerCols(int v)
    {
	Marker::cols = v;
	setMarkersDimensions(Marker::rows, Marker::cols);
    }
    
    void ArucoColor::setMarkerRows(int v)
    {
	Marker::rows = v;
	setMarkersDimensions(Marker::rows, Marker::cols);
    }
    
    void ArucoColor::setSaturationThreshold(int v)
    {
	saturationThreshold = v;
    }
    
    void ArucoColor::setValueThreshold(int v)
    {
	valueThreshold = v;
    }
    
    void ArucoColor::setAdaptiveThresholdBlockSize(int v)
    {
	adaptiveThresholdBlockSize = v;
    }
    
    void ArucoColor::setAdaptiveThresholdConstant(int v)
    {
	adaptiveThresholdConstant = v;
    }
    
    void ArucoColor::setMinMarkerArea(int v)
    {
	minMarkerArea = v;
    }
    
    void ArucoColor::setMaxMarkerArea(int v)
    {
	maxMarkerArea = v;
    }
    
    void ArucoColor::setMaxHueDeviation(int v)
    {
	maxHueDeviation = v;
    }
    
    void ArucoColor::setMaxMarkerRange(int v)
    {
	maxMarkerRange = v;
    }            

    int ArucoColor::getMarkerCols()
    {
	return Marker::cols;
    }
    
    int ArucoColor::getMarkerRows()
    {
	return Marker::rows;
    }
    
    int ArucoColor::getSaturationThreshold()
    {
	return saturationThreshold;
    }
    
    int ArucoColor::getValueThreshold()
    {
	return valueThreshold;
    }
    
    int ArucoColor::getAdaptiveThresholdBlockSize()
    {
	return adaptiveThresholdBlockSize;
    }
    
    int ArucoColor::getAdaptiveThresholdConstant()
    {
	return adaptiveThresholdConstant;
    }
    
    int ArucoColor::getMinMarkerArea()
    {
	return minMarkerArea;
    }
    
    int ArucoColor::getMaxMarkerArea()
    {
	return maxMarkerArea;
    }
    
    int ArucoColor::getMaxHueDeviation()
    {
	return maxHueDeviation;
    }
    
    int ArucoColor::getMaxMarkerRange()
    {
	return maxMarkerRange;
    }
    
    std::vector<std::pair<int,int>> ArucoColor::getDictionary()
    {
	return dictionary.getCodes();
    }
    
    std::vector<int> ArucoColor::getReferenceHues()
    {
	return Marker::referenceHues;
    }    
}

