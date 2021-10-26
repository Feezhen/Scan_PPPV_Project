#include <iostream>
#include <string>

#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <cmath>
#include <iomanip>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;
using namespace chrono;

class CAMERA
{
public:
    CAMERA(int camid, int width, int height);

    void update_img();
    int cnt_xianshi=0;
    int _width;
    int _height;
    int flag_height=0;

    void get_img(std::vector<uint8_t> &dst);
    
private:
    std::vector<uint8_t> _data;
    int _camid;
    mutex _mtx;
};

