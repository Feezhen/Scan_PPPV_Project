#include "commonFunc.h"
/************************************************************************
FunName: getBorderImg
Funtion: 图像扩展边界，扩展值为0
Parspecification:
--input
srcImg---输入源图像CV_8U或则CV_32F
borderImg---扩展后的图像
borderW--扩展边界的宽度
--ouput
NULl
DatoDevelopment:
2015/07/22
*************************************************************************/

void getBorderImg(const Mat srcImg, Mat &borderImg, const int borderW)
{
	if (borderW <= 0)
		return;
	if (srcImg.type() == CV_8U)
	{
		borderImg = Mat(srcImg.rows + 2 * borderW, srcImg.cols + 2 * borderW, CV_8U);

		int i, j;

		for (i = 0; i < srcImg.rows + 2 * borderW; i++)
		{
			for (j = 0; j < srcImg.cols + 2 * borderW; j++)
			{
				if (i < borderW || i >= srcImg.rows + borderW || j < borderW || j >= srcImg.cols + borderW)
				{
					borderImg.at<unsigned char>(i, j) = 0;
				}
				else
				{
					borderImg.at<unsigned char>(i, j) = srcImg.at<unsigned char>(i - borderW, j - borderW);
				}
			}
		}
	}
	else if (srcImg.type() == CV_32F)
	{
		borderImg = Mat(srcImg.rows + 2 * borderW, srcImg.cols + 2 * borderW, CV_32F);

		int i, j;

		for (i = 0; i < srcImg.rows + 2 * borderW; i++)
		{
			for (j = 0; j < srcImg.cols + 2 * borderW; j++)
			{
				if (i < borderW || i >= srcImg.rows + borderW || j < borderW || j >= srcImg.cols + borderW)
				{
					borderImg.at<float>(i, j) = 0.f;
				}
				else
				{
					borderImg.at<float>(i, j) = srcImg.at<float>(i - borderW, j - borderW);
				}
			}
		}

	}
}