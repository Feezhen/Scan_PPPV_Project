#include "utils.h"

using namespace std;

string int2str(int val)
{
    ostringstream out;
    out << val;
    return out.str();
}

int str2int(const string& val)
{
    istringstream in(val.c_str());
    int ret = 0;
    in >> ret;
    return ret;
}

string getCurrentTime()
{
    time_t t = time(NULL);
    char ch[64] = {0};
    strftime(ch, sizeof(ch) - 1, "%Y-%m-%d-%H-%M", localtime(&t));
    return ch;
}


//自定义一个窗口显示多图函数
void imshowMany(const std::string& _winName, const vector<cv::Mat>& ployImages)
{
	int nImg = (int)ployImages.size();//获取在同一画布中显示多图的数目  


	cv::Mat dispImg;


	int size;
	int x, y;
	//若要在OpenCV实现同一窗口显示多幅图片，图片要按矩阵方式排列，类似于Matlab中subplot();     
	//多图按矩阵排列的行数  ，h: 多图按矩阵排列的的数    
	int w, h;


	float scale;//缩放比例  
	int max;


	if (nImg <= 0)
	{
		printf("Number of arguments too small....\n");
		return;
	}
	else if (nImg > 12)
	{
		printf("Number of arguments too large....\n");
		return;
	}


	else if (nImg == 1)
	{
		w = h = 1;
		size = 400;
	}
	else if (nImg == 2)
	{
		w = 2; h = 1;//2x1  
		size = 400;
	}
	else if (nImg == 3 || nImg == 4)
	{
		w = 2; h = 2;//2x2  
		size = 400;
	}
	else if (nImg == 5 || nImg == 6)
	{
		w = 3; h = 2;//3x2  
		size = 300;
	}
	else if (nImg == 7 || nImg == 8)
	{
		w = 4; h = 2;//4x2  
		size = 300;
	}
	else
	{
		w = 4; h = 3;//4x3  
		size = 200;
	}


	dispImg.create(cv::Size(100 + size*w, 30 + size*h), CV_8UC3);//根据图片矩阵w*h，创建画布，可线的图片数量为w*h  


	for (int i = 0, m = 20, n = 20; i<nImg; i++, m += (20 + size))
	{
		x = ployImages[i].cols; //第(i+1)张子图像的宽度(列数)  
		y = ployImages[i].rows;//第(i+1)张子图像的高度（行数）  


		max = (x > y) ? x : y;//比较每张图片的行数和列数，取大值  
		scale = (float)((float)max / size);//计算缩放比例  


		if (i%w == 0 && m != 20)
		{
			m = 20;
			n += 20 + size;
		}


		cv::Mat imgROI = dispImg(cv::Rect(m, n, (int)(x / scale), (int)(y / scale))); //在画布dispImage中划分ROI区域  
		cv::resize(ployImages[i], imgROI, cv::Size((int)(x / scale), (int)(y / scale))); //将要显示的图像设置为ROI区域大小  
	}
	cv::namedWindow(_winName);
	cv::imshow(_winName, dispImg);
}