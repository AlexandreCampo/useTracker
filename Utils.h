
#ifndef UTILS_H
#define UTILS_H


#include <opencv2/highgui.hpp>


void SaveMatToPNG (cv::Mat& mat, const char* filename);
void SeedFill (cv::Mat& marked, cv::Mat& map, cv::Mat& mask, int x, int y, unsigned char target);
void EqualizeToRGB (cv::Mat& src, cv::Mat& dst);
void EqualizeBW (cv::Mat& src, cv::Mat& dst);

void Thinning(const cv::Mat& src, cv::Mat& dst);
void ThinningIteration(cv::Mat& img, int iter);


#endif
