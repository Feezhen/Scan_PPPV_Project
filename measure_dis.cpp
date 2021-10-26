#include <QThread>
#include <QDebug>
#include <QMessageBox>
#include <unistd.h>

#include "measure_dis.h"

using namespace std;
using namespace cv;

#define LEDPin 21
#define GuangdianPin1 25
#define GuangdianPin2 24

measure_dis::measure_dis(QObject *parent):
    QObject(parent)
{
    wiringPiSetup();
    pinMode(LEDPin, OUTPUT);
    digitalWrite(LEDPin, LOW);
    pinMode(GuangdianPin1,INPUT);
    pinMode(GuangdianPin2,INPUT);
    // Cam1.setExposureTime(600); //设置快门时间,单位微秒
    
    _working =false;
    _abort = false;
    capture_left = 1;
    capture_right = 1;
    // Img_vector((2));
    // Img_count = 0;
}

void measure_dis::startwork()
{
    // 读取距离,给检测标志位赋值
    qDebug() << "the measure work starts " << thread()->currentThreadId();
    // static int count = 0;
    int iCount = 1;
    int capture_left_tmp = 1;
    int capture_right_tmp = 1;
    

    // Cam1.open(0);
    // if(!Cam1.isOpened())
    // {
    //     QMessageBox::information(NULL, "Error accessing the webcam", "There was an error when trying to acces the webcam");
    //     return;
    // }
    /*
    CameraSoftTrigger(Cam1.hCamera);
    mutex.lock();
    for(int i = 0; i < iCount; i++)
    {
        Cam1 >> matOriginal;
        if (!matOriginal.empty())
        {
            // imshow("frame", frame);
            cout << "not empty" << endl;
        }
        else
        {
            cout << "empty" << endl;
            return;
        }
    }
    mutex.unlock();
    */

    mutex.lock();
    _working = true;
    _abort = false;
    mutex.unlock();

    bool readyFlag = false;

    while(1) // 循环测距
    {
        mutex.lock();
        bool abort = _abort;
        mutex.unlock();
        if (abort) 
        {
            qDebug()<<"Aborting worker process in Thread "<<thread()->currentThreadId();
            break;
        }
        // Cam1.read(matOriginal);
        
        mutex.lock();
        capture_left = digitalRead(GuangdianPin1);
        capture_right = digitalRead(GuangdianPin2);
        capture_left_tmp = capture_left;
        capture_right_tmp = capture_right;
        mutex.unlock();
        if (capture_left_tmp && capture_right_tmp)
        {
            readyFlag = true;
        }
 
        if(capture_left_tmp ==0 && capture_right_tmp == 0 && readyFlag) // 检测到了手掌 capture_left_tmp ==0 && capture_right_tmp == 0 && readyFlag
        {
            qDebug()<<"open LED!!!!!!!!!!!!! ";
            digitalWrite(LEDPin, HIGH); //打光
            // 保存图片到队列里面
            // mutex.lock();
            // detected_palm = true;
            // mutex.unlock();
            /*
            CameraSoftTrigger(Cam1.hCamera);
            mutex.lock();
            for(int i = 0; i < iCount; i++)
            {
                Cam1 >> matOriginal;
            }
            if(Img_count < 2){
                Img_vector.push_back(matOriginal);
                Img_count++;
            }
            mutex.unlock();
            */
            sleep(1);
            digitalWrite(LEDPin, LOW);//关闭光源
            emit detected_palm();//发送信号给主线程
            readyFlag = false;
            // cv::waitKey(100);
            // This will stupidly wait 1 sec doing nothing...
            // QEventLoop loop;
            // QTimer::singleShot(30, &loop, SLOT(quit()));
            // loop.exec();
            
        }
        
        // count++;
        // if (count == 1000)
        //     count = 0;
        // else // 没检测到
        // {
        //     mutex.lock();
        //     detected_palm = false;
        //     mutex.unlock();
        // }
    }

    mutex.lock();
    _working = false;
    mutex.unlock();
    digitalWrite(LEDPin, LOW);

    qDebug()<<"Worker process finished/aborted in Thread "<<thread()->currentThreadId();
    emit finished();
    // return;
}

void measure_dis::stopwork()
{
    // 停止工作
    mutex.lock();
    if(_working)
    {
        _abort = true;
        qDebug()<<"Request measurement to stopwork "<<thread()->currentThreadId();
    }
    mutex.unlock();

    
    // return;
}

cv::Mat measure_dis::get_threadImage()
{
    return matOriginal.clone();
}

void measure_dis::complete_oneImg()
{
    qDebug() << "Complete one Img and remove it from Vector" << thread()->currentThreadId();
    // mutex.lock();
    // Img_vector.pop_back(); //删掉最后一个
    // Img_count--;
    // mutex.unlock();
}
