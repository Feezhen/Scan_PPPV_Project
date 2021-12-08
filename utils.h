#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>
#include "string"
#include "vector"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

string int2str(int val);
int str2int(const string& val);
string getCurrentTime();
void imshowMany(const std::string& _winName, const vector<cv::Mat>& ployImages);


#endif
