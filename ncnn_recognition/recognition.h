#ifndef RECOGNITION_H
#define RECOGNITION_H

#include "NCNN/include/ncnn/net.h"
#include "NCNN/include/ncnn/mat.h"
#include "NCNN/include/ncnn/net.h"
#include "opencv2/opencv.hpp"

#include "MobileNet_v2_20201217.id.h"
#include "MobileNet_v2_20201217.mem.h"

// #define MODELPARAM "/home/pi/Palm/qt_scanpalm/ncnn_recognition/MobileNet_v2.param"
// #define MODELBIN "/home/pi/Palm/qt_scanpalm/ncnn_recognition/MobileNet_v2.bin"


class ncnn_net{
public:

    ncnn::Net net;

    ncnn_net();
    void net_init();
    ncnn::Mat extract_feature(cv::Mat bgr);

};


#endif



