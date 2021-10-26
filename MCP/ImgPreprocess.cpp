#include "ImgPreprocess.h"

//#include "..\\GRGFingerVenaIDAlgorithm.h"
//#include "AnisotropicDiffusionFilter.h"

/************************************************************************
FunName: NormGray
Funtion: �Ҷȹ�һ����0-255
Parspecification:
--input
srcImg---����ͼ��unsign char )
dstImg---��һ�����ͼ��
--ouput
--�Ҷȹ�һ���ɹ�����0�����򷵻�-1��
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
Funtion: �Ҷȹ�һ����0-1
Parspecification:
--input
srcImg---����ͼ��unsign char )
dstImg---��һ�����ͼ��float��
--ouput
--�Ҷȹ�һ���ɹ�����0�����򷵻�-1��
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
Funtion: ͼ��Ԥ����ӿں���
Parspecification:
--input
srcImg---����ɼ���ͼ��
dstImg---Ԥ������ͼ��
--ouput
--Ԥ����ɹ�����0�����򷵻�-1��
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
	//�ߴ��һ��
	resize(srcImg,sizeNormImg,Size(NORM_WIDTH,NORM_HEIGHT));
    //ֱ��ͼ���⻯
//    equalizeHist(sizeNormImg, sizeNormImg);
	//�Ҷȹ�һ��0-255
	ret = NormGray256(sizeNormImg,sizeNormImg);
	//���������˲� ���Ǹ�˹�˲�
	//AnisotropicDiffusionFilter(sizeNormImg,dstImg);
	GaussianBlur(sizeNormImg, dstImg, Size(5, 5), 0, 0);
//	imshow("����ǰ", srcImg);
//	imshow("��һ��", sizeNormImg);
//	imshow("�����", dstImg);
//	waitKey();

	return ret;
}