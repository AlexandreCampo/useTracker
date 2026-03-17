
#include <opencv2/highgui.hpp>

#include "Capture.h"
#include "Parameters.h"

#include <vector>
#include <iostream>


cv::Mat CalculateBackgroundMedian (Capture* capture, float startTime, float endTime, unsigned int framesCount, unsigned char lowThreshold, unsigned char highThreshold);
cv::Mat CalculateBackgroundMean (Capture* capture, float startTime, float endTime, unsigned int framesCount, unsigned char lowThreshold, unsigned char highThreshold);
