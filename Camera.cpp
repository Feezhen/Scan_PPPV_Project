#include "Camera.h"
#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <cmath>
#include <iomanip>
#include <sys/stat.h> 　
#include <sys/types.h>
#include <unistd.h>
int unlink(const char *pathname);

CAMERA::CAMERA(int camid, int width, int height)
{
    cout << "width " << width << " , height " << height << endl;
    _width = width;
    _height = height;
    _camid = camid;
    
    // 初始化大小
    //_data.resize(_width * _height * 3);
    if(_camid==0){
    _data.resize(640 * 480 * 3); 
    }else{
    _data.resize(_width * _height * 3);
    }
    
}

void CAMERA::update_img()
{

    VideoCapture capture(_camid);
    //Integer fourcc = VideoWriter.fourcc('M','J','P','G');
    capture.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
    
    if(_camid==0){
            cout<<"GAIN"<<capture.get(CV_CAP_PROP_GAIN)<<endl; 
            cout<<"FRAME_WIDTH"<<capture.get(CV_CAP_PROP_FRAME_WIDTH)<<endl;
        
            cout<<"FRAME_HEIGHT"<<capture.get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;
            
            cout<<"_FPS"<<capture.get(CV_CAP_PROP_FPS)<<endl;
            
            cout<<"BRIGHTNESS"<<capture.get(CV_CAP_PROP_BRIGHTNESS)<<endl;
            
            cout<<"CONTRAST"<<capture.get(CV_CAP_PROP_CONTRAST)<<endl;
            
            cout<<"SATURATION"<<capture.get(CV_CAP_PROP_SATURATION)<<endl;
            
            cout<<"HUE"<<capture.get(CV_CAP_PROP_HUE)<<endl;
            
            cout<<"EXPOSURE"<<capture.get(CV_CAP_PROP_EXPOSURE)<<endl; 
            
            cout<<"FRAME_COUNT"<<capture.get(CV_CAP_PROP_FRAME_COUNT)<<endl;//视频帧数

        //             FRAME_WIDTH640
//         FRAME_WIDTH640
// FRAME_HEIGHT400
// _FPS30
// BRIGHTNESS0.5
// CONTRAST0.5
// SATURATION0.5
// HUE0.5
// EXPOSURE0
// FRAME_COUNT-1
            //capture.set(CV_CAP_PROP_FOURCC, CV_VideoWriter_fourcc(*'MJPG'));


            capture.set(CV_CAP_PROP_FPS, 30);
            capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);//宽度 320
            capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);//高度240
            capture.set(CV_CAP_PROP_BRIGHTNESS,0.6);//亮度 10.53125
            capture.set(CV_CAP_PROP_CONTRAST,0.210526);//对比度 40  0.210526   0.08
            capture.set(CV_CAP_PROP_SATURATION, 0);//饱和度 50
            capture.set(CV_CAP_PROP_HUE, 0.5);//色调 50
            capture.set(CV_CAP_PROP_AUTO_EXPOSURE,0.25);
            capture.set(CV_CAP_PROP_EXPOSURE,0.1);//曝光 50  0.0048   0  藍光0.008 0.35) 0.0116583
            capture.set(cv::CAP_PROP_GAIN,0.0669456);       //增益 0.0669456         0.1   -1                                //增益 0.0669456
            
            cout<<"GAIN"<<capture.get(CV_CAP_PROP_GAIN)<<endl; 
            cout<<"FRAME_WIDTH"<<capture.get(CV_CAP_PROP_FRAME_WIDTH)<<endl;
        
            cout<<"FRAME_HEIGHT"<<capture.get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;
            
            cout<<"_FPS"<<capture.get(CV_CAP_PROP_FPS)<<endl;
            
            cout<<"BRIGHTNESS"<<capture.get(CV_CAP_PROP_BRIGHTNESS)<<endl;
            
            cout<<"CONTRAST"<<capture.get(CV_CAP_PROP_CONTRAST)<<endl;
            
            cout<<"SATURATION"<<capture.get(CV_CAP_PROP_SATURATION)<<endl;
            
            cout<<"HUE"<<capture.get(CV_CAP_PROP_HUE)<<endl;
            
            cout<<"EXPOSURE"<<capture.get(CV_CAP_PROP_EXPOSURE)<<endl; 
            
            cout<<"FRAME_COUNT"<<capture.get(CV_CAP_PROP_FRAME_COUNT)<<endl;//视频帧数

            
            this_thread::sleep_for(chrono::milliseconds(1000/30));
        }else{
            capture.set(CV_CAP_PROP_FPS, 30);
            capture.set(CV_CAP_PROP_FRAME_WIDTH, _width);//宽度 
            capture.set(CV_CAP_PROP_FRAME_HEIGHT, _height);//高度
            capture.set(CV_CAP_PROP_BRIGHTNESS,0.5);//亮度 1 0.53125 0.6 0.35
            capture.set(CV_CAP_PROP_CONTRAST,0.210526);//对比度 0.3 0.210526
            capture.set(CV_CAP_PROP_SATURATION, 0);//饱和度 50
            capture.set(CV_CAP_PROP_HUE, 0.5);//色调 50
            capture.set(CV_CAP_PROP_AUTO_EXPOSURE,0.25);
            capture.set(CV_CAP_PROP_EXPOSURE,0.0024);//曝光 50
            capture.set(cv::CAP_PROP_GAIN,0.0669456);       //增益 0.0669456
            this_thread::sleep_for(chrono::milliseconds(1000/30));
        } 
    

    Mat m;
    Mat result;
    
    //<<"baoguang:"<<capture.get(CV_CAP_PROP_EXPOSURE)<<endl;
   // this_thread::sleep_for(chrono::milliseconds(1000));
    //cout<<"111111111111111111111111111111"<<endl;
    //this_thread::sleep_for(chrono::milliseconds(1000));
   
    this_thread::sleep_for(chrono::milliseconds(100));
    int count = 0;
    int cnt=0;
    int cnt_1=0;
    while (1)
    {
        
        
    //     if(flag_height==1){
        
    //     //cout<<"过低，改变曝光"<<endl;
    //     capture.set(CV_CAP_PROP_AUTO_EXPOSURE,0.25);
    //     capture.set(CV_CAP_PROP_EXPOSURE,0.008);//曝光 50  0.0023
    //    }
        //cout<<"EXPOSURE:"<<capture.get(CV_CAP_PROP_EXPOSURE)<<endl;
        // 读取摄像头，构造虚假数据
        //cv::Mat m = cv::Mat::zeros(_height, _width, CV_8UC3);
        //  if(_camid==0){
        // capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);//宽度 
        // capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);//高度
        // capture.set(CV_CAP_PROP_EXPOSURE,-1);
        // }
      // m.at<Vec3b>(0, 0)[0] = 0;
        
       // m.at<Vec3b>(0, 0)[0] = rand()%255;
        // 拷贝数据
        //unique_lock<mutex> lock(_mtx);
        //auto start = system_clock::now();     
        capture >> m;
        unique_lock<mutex> lock1(_mtx);
        if(_camid==1){
            
            cnt_xianshi++;
            
            if(cnt_xianshi>5){
                cnt_1++;
                //unlink("/root/code/test/ncnn_test/build_new/shuju/xianshi/xianshi.jpg");
                imwrite("/root/code/test/ncnn_test/bb2/shuju/xianshi"+std::to_string(cnt_1)+".jpg",m);//+std::to_string(cnt_xianshi)
                cnt_xianshi=0;


            
            }
        }
        lock1.unlock();

        
        unique_lock<mutex> lock(_mtx);
        
        
        //memcpy(_data.data(), m.data, _data.size());
        memcpy(_data.data(), m.data, _data.size());
        lock.unlock();
        // auto end   = system_clock::now();
        // auto duration = duration_cast<microseconds>(end - start);
        // cout <<  "花费了"
        //     << double(duration.count()) * microseconds::period::num / microseconds::period::den
        //     << "秒" << endl;
    //     if(_camid==0){
    //     cnt++;
    //     imwrite("/root/code/test/ncnn_test/build_new/shuju/ceshi_pp/"+std::to_string(cnt)+".jpg",m);
    // }
       
    
        //waitKey(30);
       /* if (count++ % 15)
        {
            imshow(to_string(_camid),m);
            waitKey(30);
        }*/
       // cout << _camid << endl;
        // 
    }
}

void CAMERA::get_img(std::vector<uint8_t> &dst)
{
    
    unique_lock<mutex> lock(_mtx);
    dst = _data;
    lock.unlock();
}

