#pragma once
#include <vector> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


Mat gabor_filter(float theta);
Mat gabor(Mat src);