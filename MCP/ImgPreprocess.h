#ifndef _IMG_PREPROCESS_H_
#define _IMG_PREPROCESS_H_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;

#define  NORM_WIDTH 128//ROI width
#define  NORM_HEIGHT 128//ROI height
int ImgPreprocess(Mat srcImg, Mat &dstImg);

#endif