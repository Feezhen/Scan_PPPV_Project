#include "ImgPreprocess.h"

//#include "..\\GRGFingerVenaIDAlgorithm.h"
//#include "AnisotropicDiffusionFilter.h"

/************************************************************************
FunName: NormGray
Funtion: 灰度归一化到0-255
Parspecification:
--input
srcImg---输入图像（unsign char )
dstImg---归一化后的图像
--ouput
--灰度归一化成功返回0，否则返回-1；
DatoDevelopment:
2015/07/16
Developers:
WangDanDan
*************************************************************************/
int NormGray256(Mat srcImg,Mat &dstImg)
{
	int ret=0;
	int i,j;
	int maxGray = 0;
	int minGray = 255;
	int range =0;
	int value =0; 
	float valueNorm = 0.f;
	for( i=0;i<srcImg.rows;i++)
	{
		for( j=0;j<srcImg.cols;j++)
		{
			value= srcImg.at<unsigned char>(i,j);
			if(value<0||value>255)
				continue;
			if(maxGray< value)
			{
				maxGray = value;
			}
			if(minGray> value)
			{
				minGray = value;
			}
		}
	}	
	range = maxGray-minGray;
	if(range==0)
	{
		ret = -1;
		return ret;
	}
	dstImg = Mat(srcImg.rows,srcImg.cols,CV_8U);
	for( i=0;i<srcImg.rows;i++)
	{
		for( j=0;j<srcImg.cols;j++)
		{
			value= srcImg.at<unsigned char>(i,j);
			valueNorm =255.0*(value-minGray)/range;
			dstImg.at<unsigned char>(i,j) = (unsigned char)valueNorm;
		}
	}
	return ret;
}

/************************************************************************
FunName: NormGray
Funtion: 灰度归一化到0-1
Parspecification:
--input
srcImg---输入图像（unsign char )
dstImg---归一化后的图像（float）
--ouput
--灰度归一化成功返回0，否则返回-1；
DatoDevelopment:
2015/07/16
Developers:
WangDanDan
*************************************************************************/
int NormGray(Mat srcImg,Mat &dstImg )
{
	int ret=0;
	int i,j;
	int maxGray = 0;
	int minGray = 255;
	int range =0;
	int value =0; 
	float valueNorm = 0.f;
	for( i=0;i<srcImg.rows;i++)
	{
		for( j=0;j<srcImg.cols;j++)
		{
			value= srcImg.at<unsigned char>(i,j);
			if(value<0||value>255)
				continue;
			if(maxGray< value)
			{
				maxGray = value;
			}
			if(minGray> value)
			{
				minGray = value;
			}
		}
	}	
	range = maxGray-minGray;
	if(range==0)
	{
		ret = -1;
		return ret;
	}
	dstImg = Mat(srcImg.rows,srcImg.cols,CV_32F);
	for( i=0;i<srcImg.rows;i++)
	{
		for( j=0;j<srcImg.cols;j++)
		{
			value= srcImg.at<unsigned char>(i,j);
			valueNorm =float(value-minGray)/range;
			dstImg.at<float>(i,j) = valueNorm;
		}
	}
	return ret;
}

/************************************************************************
FunName: ImgPreprocess
Funtion: 图像预处理接口函数
Parspecification:
--input
srcImg---输入采集的图像
dstImg---预处理后的图像
--ouput
--预处理成功返回0；否则返回-1；
DatoDevelopment:
2015/07/16
Developers:
WangDanDan
*************************************************************************/

int ImgPreprocess(Mat srcImg, Mat &dstImg)
{
	int ret =0;
	int srcWidth = srcImg.cols;
	int srcHeight = srcImg.rows;
	if(srcImg.empty()||srcWidth<=0||srcHeight<=0)
	{
		ret=-1;
		return ret;
	}
	Mat sizeNormImg= Mat(NORM_WIDTH,NORM_HEIGHT,srcImg.type());
	//尺寸归一化
	resize(srcImg,sizeNormImg,Size(NORM_WIDTH,NORM_HEIGHT));
    //直方图均衡化
//    equalizeHist(sizeNormImg, sizeNormImg);
	//灰度归一化0-255
	ret = NormGray256(sizeNormImg,sizeNormImg);
	//各项异性滤波 考虑高斯滤波
	//AnisotropicDiffusionFilter(sizeNormImg,dstImg);
	GaussianBlur(sizeNormImg, dstImg, Size(5, 5), 0, 0);
//	imshow("处理前", srcImg);
//	imshow("归一化", sizeNormImg);
//	imshow("处理后", dstImg);
//	waitKey();

	return ret;
}