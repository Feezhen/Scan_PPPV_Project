#ifndef _COMMONFUNC_H_
#define _COMMONFUNC_H_
#include "opencv2/opencv.hpp"
using namespace cv;
void getBorderImg(const Mat srcImg, Mat &borderImg, const int borderW);
#endif
