
#include "arucoColor.h"

// #include <iostream>
// #include <vector>


using namespace cv; 
using namespace std;


ArucoColor::ArucoColor()
{
    // init data structures for image segmentation   
    hsvMin[0] = 0;
    hsvMin[1] = SEGSTHRESHOLD;
    hsvMin[2] = SEGVTHRESHOLD;
    
    hsvMax[0] = 255;
    hsvMax[1] = 255;
    hsvMax[2] = 255;
    
    structuringElement = getStructuringElement(
	MORPH_ELLIPSE,
	Size (2 * DILATE + 1, 2 * DILATE + 1),
	Point (DILATE, DILATE)
	);
}

ArucoColor::~ArucoColor()
{

}

void ArucoColor::setDictionary(std::vector<int> codes)
{
    dictionary = new Dictionary(Marker::numPixels, Marker::referenceHues);
    
    RNG rng(12345);

    for (int i = 0; i < codes.size(); i++)
    {
	dictionary->add(codes[i], i);

	Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	drawingColors.push_back(color);
    }
}

void ArucoColor::setMarkersDimensions (int numRows, int numCols)
{
    // todo right now only circular markers
    Marker::cols = numCols;
    Marker::rows = numRows;
    Marker::numPixels = numCols * numRows;
}

void ArucoColor::setReferenceHues(std::vector<int> hues)
{
    Marker::referenceHues = hues;
    Marker::numHues = hues.size();
    
    // fill the distances table
    Marker::distances.clear();
    for (int i = 0; i < hues.size(); i++)
    {
	if (i <= hues.size() / 2)
	    Marker::distances.push_back(i);
	else
	    Marker::distances.push_back(hues.size() - i);	
    }

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
    adaptiveThreshold(grey, segmented, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, ATBLOCK, ATCONST);

    // use negative version for better detection
    bitwise_not(segmented, segmented);

    // detect colors
    cvtColor(img, hsv, CV_BGR2HSV);
        
    inRange(hsv, hsvMin, hsvMax, segmented2);

    // erode & dilate colors
    if (DILATE > 0)
    {
	dilate (segmented2, segmented3, structuringElement);	    
    }

    // merge detection of colors and white borders
    segmented |= segmented3;
}

void ArucoColor::findRectangles(Mat& seg)
{
    // now extract contours
    vector<vector<Point> > contours;
    findContours(seg, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
    // Find the convex hull object for each contour
    hulls.clear();
    vector<Point> centers;
    vector<double> areas;
    for( int i = 0; i < contours.size(); i++ )
    {
	double area = contourArea(contours[i]);

	if (area > MINMARKERAREA && area < MAXMARKERAREA)
	{	    
	    // convex hull produces a lot of lines
	    // min area rect finds rects everywhere
	    // approxPolyDP seems best method here
	    vector<Point> h;
	    approxPolyDP (contours[i], h, double(contours[i].size()) * POLYTOLERANCE, true);
	    
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
			int dx = centers[j].x - center.x;
			int dy = centers[j].y - center.y;
			int d = dx*dx + dy*dy;
			
			// same location
			if (d < 16 * 16)
			{				
			    assigned = true;
			    if (area < areas[j])
			    {
				hulls[j] = h;
				centers[j] = center;
				areas[j] = area;
			    }
			}
		    }
		    if (!assigned)
		    {
			centers.push_back(center);
			areas.push_back(area);
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
		
		if (pix[1] > STHRESHOLD && pix[2] > VTHRESHOLD)
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
	cv::putText(img, str.str(), pt, CV_FONT_HERSHEY_PLAIN, 3, Scalar (0,0,250), 2, CV_AA);		    
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
	warp(hsv, markerImage, markerImage.size(), hulls[i]);

	Marker candidate;
	if (decodeMarker(markerImage, candidate))
	{	    
	    // find best marker(s)
	    int dist;
	    int id = dictionary->findBestMarker(candidate.hues, MAXHUEDIST, MAXMARKERDIST, dist);
	    
	    if (id >= 0)
	    {
		// record detected marker
		detectedMarkers.push_back(DetectedMarker(id, hulls[i]));
	    }				    
	}
    }
    return detectedMarkers;
}
