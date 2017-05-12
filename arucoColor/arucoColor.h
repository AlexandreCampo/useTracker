
#ifndef ARUCO_COLOR
#define ARUCO_COLOR

#include "marker.h"
#include "dictionary.h"

#include <vector>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


namespace ac
{
    
class ArucoColor
{
public:

    struct DetectedMarker
    {
	cv::Point center;
	std::vector<cv::Point> corners;
	int id;

	DetectedMarker(int id, std::vector<cv::Point>& corners)
	    {
		this->id = id;
		this-> corners = corners;
	    }
    };

    
    ArucoColor();
    ~ArucoColor();
    
    void setDictionary (std::vector<int> codes);
    void setReferenceHues (std::vector<int> hues);
    
    std::vector<DetectedMarker>& detect (const cv::Mat& img);
    void drawMarkers(cv::Mat& img);
    
    void setMarkersDimensions (int numRows, int numCols);


    cv::Mat segmented;
    
private:

    // various parameters to ease detection
    int STHRESHOLD = 60;
    int VTHRESHOLD = 100;
    
    int SEGSTHRESHOLD = 60;
    int SEGVTHRESHOLD = 100;

//    int MINCONTOURSIZE = 0;

    int ATBLOCK = 13;
    int ATCONST = -5;

    float POLYTOLERANCE = 0.1;

    float MINMARKERAREA = 150;
    float MAXMARKERAREA = 1600;

    int MAXHUEDIST = 25;
    int MAXMARKERDIST = 4;

//    int DILATE = 2;


    // markers characteristics
//    int ncol = 9;

    /* int markersNumHues = 9; */
    /* int markersNumCols = 2; */
    /* int markersNumRows = 2; */
    /* int markersNumPixels = 4; */
    
    std::vector<Marker> dictionary2;
    Dictionary* dictionary;

//    int dictMarkerIDs[] = {5880, 5601, 5261, 2411, 1749, 191, 6431, 1530, 3165, 4986, 3597, 4387, 3522, 1069, 316, 5661, 5276, 3843, 2584, 3380, 5815, 6337, 3473, 1345, 6158, 5809, 4024, 5350, 905, 2147, 1401, 4165, 676, 5306, 3135, 3814, 5938, 4858, 3393, 1212, 1142, 2626, 4401, 5334, 6111, 294, 4792, 254, 4678, 3859, 4534, 4636, 481, 3026, 2154, 3021, 3660, 5410, 5773, 6007, 5777, 4590, 1600, 1920, 3808};
    
    std::vector<cv::Scalar> drawingColors;

    std::vector<DetectedMarker> detectedMarkers;

    // members for segmentation
    cv::Mat grey;
    cv::Mat hsv;
    cv::Mat segmented2;
    cv::Mat segmented3;
    cv::Vec3b hsvMin;
    cv::Vec3b hsvMax;
    cv::Mat structuringElement;

    // members for rectangle detection
    std::vector<std::vector<cv::Point>> hulls;

    
    void segmentImage (const cv::Mat& img);
    void warp(const cv::Mat &in, cv::Mat &out, cv::Size size, std::vector<cv::Point> points);
    int findBestMarker(Marker& m);
    void findRectangles(cv::Mat& seg);
    bool decodeMarker(const cv::Mat& marker, Marker& candidate);

};

}
#endif
