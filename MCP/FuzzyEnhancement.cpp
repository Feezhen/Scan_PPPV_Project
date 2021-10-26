#include "FuzzyEnhancement.h"
//#include "..\preprocess\CommonAlgorithms.h"
#include "commonFunc.h"
void memberShipFunc(const Mat srcImg,Mat &memshipImg)
{
	memshipImg = Mat(srcImg.rows,srcImg.cols,CV_32F);
	for(int i=0;i<srcImg.rows;i++)
	{
		for(int j=0;j<srcImg.cols;j++)
		{
			memshipImg.at<float>(i,j) = float(srcImg.at<uchar>(i,j))/255;
		//	printf("%f ",memshipImg.at<float>(i,j));
		}
	}
	Mat tmpImg;
	int width=10; 	
	getBorderImg(memshipImg, tmpImg, width);
	//再次计算隶属度
	float sum =0.f;
	float avg = 0.f;
	for(int i=width;i<memshipImg.rows+width; i++)
	{
		for(int j=width;j<memshipImg.cols+width; j++)
		{
			sum =0.f;
			for(int m=-width;m<width;m++)
			{
				for(int n=-width;n<width;n++)
				{
					float tmp = tmpImg.at<float>(i+m,j+n);
					sum+=tmp;
				}
			}
			avg = sum/((2*width+1)*(2*width+1));
			tmpImg.at<float>(i,j) = avg;
		}
	}

	for(int i=0;i<memshipImg.rows;i++)
	{
		for(int j=0;j<memshipImg.cols;j++)
		{
			float u = memshipImg.at<float>(i,j);
			float avg = tmpImg.at<float>(i+width,j+width);
			//printf("%f %f \n",u, avg);
			if(u<avg&& u>=0)
			{
				//memshipImg.at<float>(i,j)=2*u*u; //空洞较多
				memshipImg.at<float>(i,j)=u*u/(avg);
			}
			else if(u>avg && u<=1)
			{
				//memshipImg.at<float>(i,j)=1-2*(1-u)*(1-u);//空洞较多

				memshipImg.at<float>(i,j)=1-(1-u)*(1-u)/(1-avg);
			}
		}
		//printf("\n");
	}
}

void fuzzyEnhance(const Mat srcImg,Mat &dstImg)
{
	Mat memshipImg;
	//计算隶属度
	memberShipFunc(srcImg,memshipImg);

	//增强图像
	dstImg = Mat(srcImg.rows,srcImg.cols,CV_8U);
	for(int i=0;i<srcImg.rows;i++)
	{
		for(int j=0;j<srcImg.cols;j++)
		{
			float u = memshipImg.at<float>(i,j);
			dstImg.at<uchar>(i,j) = (unsigned char)255*u;
		}
	}

}
