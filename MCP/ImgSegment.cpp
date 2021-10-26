#include "ImgSegment.h"
//#include "NiBlackThreshold.h"
#include "FuzzyEnhancement.h"
//#include "..\preprocess\CommonAlgorithms.h"
#include "commonFunc.h"
//8个方向算子
int dir0[81]={
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	3,  0, -1,  0,  -4, 0, -1,  0,  3,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0 
};

int dir1[81]={
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	3,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0, -1,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0, -4,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, -1,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  3,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0 
}; 

int dir2[81]={
	3,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0, -1,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0, -4,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, -1,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  3 
};

int dir3[81]={
	0,  0,  3,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, -1,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0, -4,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0, -1,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  3,  0,  0 
};

int dir4[81]={
	0,  0,  0,  0,  3,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  -1,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0, -4,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  -1,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  3,  0,  0,  0,  0 
};

int dir5[81]={
	0,  0,  0,  0,  0,  0,  3,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0, -1,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0, -4,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, -1,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  3,  0,  0,  0,  0,  0,  0 
};

int dir6[81]={
	0,  0,  0,  0,  0,  0,  0,  0,  3,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, -1,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0, -4,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0, -1,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	3,  0,  0,  0,  0,  0,  0,  0,  0 
};

int dir7[81]={
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  3,
	0,  0,  0,  0,  0,  0, -1,  0,  0,
	0,  0,  0,  0, -4,  0,  0,  0,  0,
	0,  0, -1,  0,  0,  0,  0,  0,  0,
	3,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0 
};
int *Dirs[8]={dir0,dir1,dir2,dir3,dir4,dir5,dir6,dir7};



 /************************************************************************
FunName: conv8Dirs
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
int conv8Dirs(Mat srcImg,Mat &dstImg)
{
	int ret=0;
	//输入图像基础上边界扩展8行8列
	int borderW = 4;
	Mat borderImg;//(srcImg.rows+8,srcImg.cols+8,srcImg.type());
	dstImg = Mat(srcImg.rows,srcImg.cols,srcImg.type());
	getBorderImg(srcImg,borderImg, borderW);
	int i,j,k,m,n;
	if(srcImg.type()==CV_32F)
	{		
		double maxGray = DBL_MIN;
		double sum = 0;
		double value=0;

		for(i=4;i<borderImg.rows-4;i++)
		{
			for(j=4;j<borderImg.cols-4;j++)
			{//每个像素和8个方向算子卷积
				maxGray = FLT_MIN;
				for(k=0;k<8;k++)
				{
					sum=0.f;
					int num=0;
					for(m=-4;m<=4;m++)
					{
						for(n=-4;n<=4;n++)
						{						
							value = borderImg.at<float>(i+m,j+n);
							sum+=value*Dirs[k][(m+4)*9+(n+4)];
						}
					}
					sum = sum/7.0;
					if(maxGray<sum)
					{
						maxGray=sum;	
					}
				}
				if(maxGray>0.f)
				{
					dstImg.at<float>(i-4,j-4)= maxGray;
				}else
				{
					dstImg.at<float>(i-4,j-4)= 0.f;
				}
			}
		}
	}
	else if(srcImg.type()==CV_8U)
	{				
		//上边缘
		for(i=0;i<borderW;i++)
		{
			for(j=borderW;j<borderImg.cols-borderW;j++)
			{			
				borderImg.at<unsigned char>(i,j) = srcImg.at<uchar>(0,j-borderW);				
			}
		}	
		//下边缘
		for(i=borderImg.rows-borderW;i<borderImg.rows;i++)
		{
			for(j=borderW;j<borderImg.cols-borderW;j++)
			{			
				borderImg.at<unsigned char>(i,j) = srcImg.at<uchar>(srcImg.rows-1,j-borderW);				
			}
		}		
		int maxGray = INT_MIN;
		int sum = 0;
		int value=0;
		
		for(i=4;i<borderImg.rows-4;i++)
		{
			for(j=4;j<borderImg.cols-4;j++)
			{//每个像素和8个方向算子卷积
				maxGray = INT_MIN;
				for(k=0;k<8;k++)
				{
					sum=0;
					for(m=-4;m<=4;m++)
					{
						for(n=-4;n<=4;n++)
						{						
							value = borderImg.at<unsigned char>(i+m,j+n);
							sum+=value*Dirs[k][(m+4)*9+(n+4)];
						}
					}	
					//sum/=7;
					if(maxGray<sum)
					{
						maxGray=sum;	
					}
				}
				if(maxGray>0)
				{	maxGray = maxGray>255? 255:maxGray;
					dstImg.at<unsigned char>(i-4,j-4)= maxGray;
				}else
				{
					dstImg.at<unsigned char>(i-4,j-4)= 0;
				}					
			}
		}
	}
	return ret;	
}

void MinimunConvMap(Mat srcImg, Mat &dstImg)
{
	int borderW = 4;
	Mat borderImg;//(srcImg.rows+8,srcImg.cols+8,srcImg.type());
	dstImg = Mat(srcImg.rows, srcImg.cols, srcImg.type());
	getBorderImg(srcImg, borderImg, borderW);

	int minValue = INT_MAX;
	int sum = 0;
	int value = 0;

	for (int i = 4; i < borderImg.rows - 4; i++)
	{
		for (int j = 4; j < borderImg.cols - 4; j++)
		{//每个像素和8个方向算子卷积
			minValue = INT_MAX;
			for (int k = 0; k < 8; k++)
			{
				sum = 0;
				for (int m = -4; m <= 4; m++)
				{
					for (int n = -4; n <= 4; n++)
					{
						value = borderImg.at<unsigned char>(i + m, j + n);
						sum += value*Dirs[k][(m + 4) * 9 + (n + 4)];
					}
				}
				//sum/=7;
				if (minValue > sum)
				{
					minValue = sum;
				}
			}
			if (minValue > 0)
			{
				minValue = minValue > 255 ? 255 : minValue;
				dstImg.at<unsigned char>(i - 4, j - 4) = minValue;
			}
			else
			{
				dstImg.at<unsigned char>(i - 4, j - 4) = 0;
			}
		}
	}
}

/************************************************************************
FunName: meanValueSegment
Funtion: 使用图像平均灰度值分割,所有灰度值大于均值的像素等于均值
Parspecification:
--input
srcImg---输入图像
--ouput
NULL
DatoDevelopment:
2015/08/04
Developers:
WangDanDan
*************************************************************************/
void meanValueSegment(Mat &srcImg)
{
	int sum=0;
	for(int i=0;i<srcImg.rows;i++)
	{
		for(int j=0;j<srcImg.cols;j++)
		{
			sum+=srcImg.at<uchar>(i,j);
		}
	}
	int mean =1.5*sum/(srcImg.rows * srcImg.cols);
	unsigned char val=0;
	for(int i=0;i<srcImg.rows;i++)
	{
		for(int j=0;j<srcImg.cols;j++)
		{
			val=srcImg.at<uchar>(i,j);
			srcImg.at<uchar>(i,j)= val>mean? mean:val;
		}
	}
}

/************************************************************************
FunName: claheEnhance
Funtion: 受限自适应直方图均衡
Parspecification:
--input
srcImg---输入图像
enhanceImg--增强图像
clipLimit---阈值？？
--ouput
NULL
DatoDevelopment:
2015/08/19
*************************************************************************/
void claheEnhance(const Mat srcImg,Mat &enhanceImg,float clipLimit)
{	
	Ptr<cv::CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(clipLimit);
	clahe->apply(srcImg,enhanceImg);	
}

/************************************************************************
FunName: ImgSegment
Funtion: 图像分割接口函数
Parspecification:
--input
srcImg---输入预处理的图像
dstImg---分割后的图像
--ouput
--分割成功返回0；否则返回-1；
DatoDevelopment:
2015/07/16
Developers:
WangDanDan
*************************************************************************/
int ImgSegment(Mat srcImg,Mat &dstImg)
{
	int ret=0;
	int srcWidth = srcImg.cols;
	int srcHeight = srcImg.rows;

	if(srcImg.empty()||srcWidth<=0||srcHeight<=0)
	{
		ret=-1;
		return ret;
	}		
	Mat featImg;
	//8方向卷积图和第一次阈值分割
 	conv8Dirs(srcImg, featImg);  
	//第二次阈值分割
	//meanValueSegment(featImg); //
	//模糊增强
	Mat enhanceImg;
	fuzzyEnhance( featImg,enhanceImg); 
	//claheEnhance(featImg,enhanceImg,40);	
	//NiBlack阈值分割
	//NiBlackThreshold(enhanceImg,dstImg);	
	dstImg = Mat(srcImg.rows,srcImg.cols,CV_8U);

	enhanceImg.copyTo(dstImg);
//	cvAdaptiveThreshold(&(enhanceImg.operator CvMat()), &(dstImg.operator CvMat()), 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 21, 5);   
 	return ret;
}

