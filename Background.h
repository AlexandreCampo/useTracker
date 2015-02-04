
#include<opencv2/highgui/highgui.hpp>
using namespace cv;

#include "Capture.h"
#include "Parameters.h"

#include <vector>
#include <iostream>


Mat CalculateBackgroundMedian (Capture* capture, float startTime, float endTime, unsigned int framesCount);
Mat CalculateBackgroundMean (Capture* capture, float startTime, float endTime, unsigned int framesCount);
