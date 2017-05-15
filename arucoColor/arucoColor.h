
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
	float area;
	cv::Point center;
	std::vector<cv::Point> corners;
	int id;

	DetectedMarker(int id, std::vector<cv::Point>& corners, cv::Point center, float area)
	    {
		this->id = id;
		this-> corners = corners;
		this->center = center;
		this->area = area;
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
    

    void setMarkerCols(int v);
    void setMarkerRows(int v);
    void setSaturationThreshold(int v);
    void setValueThreshold(int v);
    void setAdaptiveThresholdBlockSize(int v);
    void setAdaptiveThresholdConstant(int v);
    void setMinMarkerArea(int v);
    void setMaxMarkerArea(int v);
    void setMaxHueDeviation(int v);
    void setMaxMarkerRange(int v);

    int getMarkerCols();
    int getMarkerRows();
    int getSaturationThreshold();
    int getValueThreshold();
    int getAdaptiveThresholdBlockSize();
    int getAdaptiveThresholdConstant();
    int getMinMarkerArea();
    int getMaxMarkerArea();
    int getMaxHueDeviation();
    int getMaxMarkerRange();
    std::vector<std::pair<int,int>> getDictionary();
    std::vector<int> getReferenceHues();


private:
    
    // various parameters to ease detection
    int saturationThreshold = 60;
    int valueThreshold = 100;

    int adaptiveThresholdBlockSize = 6;
    int adaptiveThresholdConstant = -5;

    float approxPolyTolerance = 0.1;

    float minMarkerArea = 150;
    float maxMarkerArea = 1600;

    int maxHueDeviation = 25;
    int maxMarkerRange = 4;
    
    Dictionary dictionary;

    std::vector<cv::Scalar> drawingColors;

    std::vector<DetectedMarker> detectedMarkers;

    // member for segmentation
    cv::Mat grey;
    
    // members for rectangle detection
    std::vector<std::vector<cv::Point>> hulls;
    std::vector<cv::Point> hullCenters;
    std::vector<double> hullAreas;

    void buildDistancesTable();
    
    void segmentImage (const cv::Mat& img);
    void warp(const cv::Mat &in, cv::Mat &out, cv::Size size, std::vector<cv::Point> points);
    int findBestMarker(Marker& m);
    void findRectangles(cv::Mat& seg);
    bool decodeMarker(const cv::Mat& marker, Marker& candidate);

};

}
#endif
