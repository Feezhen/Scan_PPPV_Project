#ifndef MEASURE_DIS_H
#define MEASURE_DIS_H

#include <iostream>
#include <stdlib.h>

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QEventLoop>
#include <QMessageBox>
#include <QDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/opencv.hpp>
#include <wiringPi.h>
// #include <opencv2/highgui.hpp>
// 自己的头文件
// #include "MVCapture.h"


class measure_dis : public QObject
{
    Q_OBJECT

public:
    explicit measure_dis(QObject *parent = 0);
    cv::Mat get_threadImage();
    void complete_oneImg();
    // friend MVSDK_API CameraSdkStatus CameraSoftTrigger(CameraHandle hCamera);

    // std::vector<cv::Mat> Img_vector = std::vector<cv::Mat>(1);
    // std::vector<cv::Mat> Img_vector;
    // Foo() : Img_vector(2)

private:
    // bool detected_palm;
    // cv::VideoCapture Cam1;
    // MVCapture Cam1;
    cv::Mat matOriginal;
    
    int Img_count;
    /**
     * @brief Process is aborted when @em true
     */
    bool _abort;
    /**
     * @brief @em true when Worker is doing work
     */
    bool _working;
    QMutex mutex;

    int capture_left;
    int capture_right;
    
    

signals:
    void detected_palm();
    void finished();

public slots:
    void startwork();
    void stopwork();

// private slots:

};


#endif