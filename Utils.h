
#ifndef UTILS_H
#define UTILS_H


#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;

void SaveMatToPNG (Mat& mat, const char* filename);
void SeedFill (Mat& marked, Mat& map, Mat& mask, int x, int y, unsigned char target);
void EqualizeToRGB (Mat& src, Mat& dst);
void EqualizeBW (Mat& src, Mat& dst);

void Thinning(const cv::Mat& src, cv::Mat& dst);
void ThinningIteration(cv::Mat& img, int iter);


#endif
