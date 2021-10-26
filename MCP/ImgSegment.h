#ifndef _IMG_SEGMENT_H_
#define _IMG_SEGMENT_H_
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
using namespace cv;

int ImgSegment(Mat srcImg,Mat &dstImg);
void MinimunConvMap(Mat srcImg, Mat &dstImg);
#endif 