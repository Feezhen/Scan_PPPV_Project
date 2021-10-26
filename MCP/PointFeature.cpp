#include "PointFeature.h"
#include "commonFunc.h"
#include <iostream>
#include "ImgPreprocess.h"
#include "ImgSegment.h"
#include <Eigen/Dense>

using namespace std;
using namespace cv;
using namespace Eigen;

void DetectFeaturePoint(Mat MCMImg, vector<Point>&featurePoints)
{
	int winW = 8, winH = 8;
	int winRows  = MCMImg.rows / winW; //row col
	int winCols = MCMImg.cols / winH;

	//Mat FeaturePointImg(MCMImg.rows, MCMImg.cols, MCMImg.type());
	Mat FeaturePointImg = Mat::zeros(MCMImg.size(), MCMImg.type());
	//分块最大值位置保存下来
	for (int i = 0; i < winRows; i++)
	{
		for (int j = 0; j < winCols; j++)
		{
			int startX = j*winW;
			int startY = i*winH;
			int maxVal = 0;
			Point maxPoint;
			for (int m = startY; m < startY + winH; m++)
			{
				for (int n = startX; n < startX + winW; n++)
				{
					if ((int)MCMImg.at<uchar>(m, n)>maxVal)
					{
						//cout << (int)MCMImg.at<uchar>(m, n) << " ";
						maxVal = (int)MCMImg.at<uchar>(m, n);
						maxPoint.x = n;
						maxPoint.y = m;
					}
				}
			}
			if (maxVal > 0)
			{
				FeaturePointImg.at<uchar>(maxPoint.y, maxPoint.x) = 255;
			}
		}
	}

	//遍历所有点的9*9邻域点-保留最小曲率点中的分块邻域最大值
	for (int i = 4; i < FeaturePointImg.rows - 4; i++)
	{
		for (int j = 4; j < FeaturePointImg.cols - 4; j++)
		{
			if (255 != (int)FeaturePointImg.at<uchar>(i, j))
			{
				continue;
			}
			int maxVal = 0;
			Point maxPoint;
			for (int m = i - 4; m <= i + 4; m++)
			{
				for (int n = j - 4; n <= j + 4; n++)
				{
					if (255 == (int)FeaturePointImg.at<uchar>(m, n))
					{
						if ((int)FeaturePointImg.at<uchar>(m, n)>maxVal)
						{
							maxVal = (int)FeaturePointImg.at<uchar>(m, n);
							maxPoint.x = n;
							maxPoint.y = m;
						}
						FeaturePointImg.at<uchar>(m, n) = 0;//将最小曲率点的点值赋值0
					}
				}
			}
			FeaturePointImg.at<uchar>(maxPoint.y, maxPoint.x) = 255;//邻域的最大值设置为255

		}
	}

	//保存特征点
	for (int i = 4; i < FeaturePointImg.rows - 4; i++)
	{
		for (int j = 4; j < FeaturePointImg.cols - 4; j++)
		{

			if (255 == (int)FeaturePointImg.at<uchar>(i, j))
			{

				featurePoints.push_back(Point(j, i));
			}
		}
	}
	return;

}

/************************************************************************
*以下为特征描述子-方向直方图
*************************************************************************/
void CalculateGradients(Mat srcImg, int * gradX, int * gradY)
{
	int borderW = 1;
	Mat borderImg;
	//dstImg = Mat(srcImg.rows, srcImg.cols, srcImg.type());
	getBorderImg(srcImg, borderImg, borderW);
	int count = 0;
	for (int i = 1; i < borderImg.rows - 1; i++)
	{
		for (int j = 1; j < borderImg.cols - 1; j++)
		{
			gradX[count] = (int)borderImg.at<uchar>(i, j + 1) - (int)borderImg.at<uchar>(i, j - 1);
			gradY[count] = (int)borderImg.at<uchar>(i - 1, j) - (int)borderImg.at<uchar>(i + 1, j);
			count++;
		}
	}
}

void featureExtractHog(const Mat &orgImg, vector<float> &featureArray)
{
	Mat orgImgtmp;

	orgImg.copyTo(orgImgtmp);
	int imgWidth = orgImgtmp.cols;
	int imgHeight = orgImgtmp.rows;
	//resize(orgImgtmp, orgImgtmp, Size(imgWidth, imgHeight));

	HOGDescriptor *hog_org = new HOGDescriptor(cvSize(imgWidth, imgHeight), cvSize(imgWidth*2/3, imgHeight*2/3),
		cvSize(imgWidth / 3, imgHeight / 3), cvSize(imgWidth / 3, imgHeight /3), 9);//构建hog类

	vector<float> descriptors_org;
	hog_org->compute(orgImgtmp, descriptors_org, Size(1, 1), Size(0, 0));	//hog特征计算

	unsigned long n = 0;
//	for (vector<float>::iterator iter = descriptors_org.begin(); iter != descriptors_org.end(); iter++)
//	{
//		featureArray[n] = (float)(*iter);
//		n++;
//	}
    featureArray = descriptors_org;
	delete hog_org;
	return;
}

void FeatureExtractHVD(Mat srcImg, int cellRows, int cellCols, float* featArray)
{
	const float pi = 3.1415926536;
	int cellWidth = srcImg.cols / cellCols;
	int cellHeight = srcImg.rows / cellRows;
	int nBins = 5;
	float binSize = pi / nBins;

	for (int i = 0; i < cellCols*cellRows*nBins; i++)
	{
		featArray[i] = 0.0;
	}

	int *gradX = new int[srcImg.rows*srcImg.cols];
	int *gradY = new int[srcImg.rows*srcImg.cols];

	CalculateGradients(srcImg, gradX, gradY);

	for (int i = 0; i < srcImg.rows; i++)
	{
		for (int j = 0; j < srcImg.cols; j++)
		{
			int dx = *(gradX + i*srcImg.rows + j);
			int dy = *(gradY + i*srcImg.rows + j);

			float gradOrient = atan2(float(dy), float(dx));
			if (gradOrient < 0)
			{
				gradOrient += pi;
			}
			gradOrient = gradOrient > 0 ? gradOrient : 0;//防止负数索引
			int bin = gradOrient / binSize;

			//cout << bin<<" "<<gradOrient << endl;
			int y = i / cellHeight;
			int x = j / cellWidth;
			featArray[(y*cellCols + x)*nBins + bin] += 1;
			
		}
	}

	delete[] gradX;
	delete[] gradY;
}

void FeaturePointDescriptor(Mat VPMImg, vector<Point> featurePoints, vector<Point> &getFeaturePoints, vector<vector<float>> &FeatureDescriptor)
{
	int featlen = FEATURE_POINT_DIM;
	int blockW = 30;
	int blockH = 24;

	int count = 0;
	for (int i = 0; i < featurePoints.size(); i++)
	{
		if (featurePoints[i].x >= blockW / 2 && featurePoints[i].x < (VPMImg.cols - blockW / 2))
		{
			if (featurePoints[i].y >= blockH / 2 && featurePoints[i].y < (VPMImg.rows - blockH / 2))
			{
				getFeaturePoints.push_back(featurePoints[i]);

				Mat localBlock = VPMImg(Rect(featurePoints[i].x - blockW / 2, featurePoints[i].y - blockH / 2, blockW, blockH));

                vector<float> featureArray;
				//FeatureExtractHVD(localBlock, 3, 3, featureArray);
				featureExtractHog(localBlock, featureArray);
				FeatureDescriptor.push_back(featureArray);
			}
		}
	}
}

float calCosineSimilarity(const vector<float>& feature1, const vector<float>& feature2, int length)
{
	int i = 0;
	double sumDot = 0.0;
	double sumNorm1 = 0.0;
	double sumNorm2 = 0.0;
	float v1, v2;
	for (int i = 0; i < length; i++)
	{
		v1 = feature1[i];
		v2 = feature2[i];
		sumDot += v1 * v2;
		sumNorm1 += v1 * v1;
		sumNorm2 += v2 * v2;
	}
	v1 = sqrtf(sumNorm1);
	v2 = sqrtf(sumNorm2);
	return sumDot / (v1 * v2);
}


void SearchCorrespondingPoint(vector<Point>& PointVector1, vector<vector<float>>& FeatureDescriptor1, vector<Point>& PointVector2, vector<vector<float>>& FeatureDescriptor2, vector<PointMATCH>& matchList)
{

	if (PointVector1.size() <= PointVector2.size())
	{
		for (int i = 0; i < PointVector1.size(); i++)
		{
			float maxSim = 0.f;
			Point matchPoint1, matchPoint2;
			for (int j = 0; j < PointVector2.size(); j++)
			{
				float sim = calCosineSimilarity(FeatureDescriptor1[i], FeatureDescriptor2[j], FEATURE_POINT_DIM);
				if (sim > maxSim)
				{
					maxSim = sim;
					matchPoint1 = PointVector1[i];
					matchPoint2 = PointVector2[j];
				}
			}
			PointMATCH savePointPair;
			savePointPair.pointSrc = matchPoint1;
			savePointPair.pointDst = matchPoint2;
			savePointPair.similarity = maxSim;
			matchList.push_back(savePointPair);
		}
	}
	else
	{
		for (int i = 0; i < PointVector2.size(); i++)
		{
			float maxSim = 0.f;
			Point matchPoint1, matchPoint2;
			for (int j = 0; j < PointVector1.size(); j++)
			{
				float sim = calCosineSimilarity(FeatureDescriptor2[i], FeatureDescriptor1[j], FEATURE_POINT_DIM);
				if (sim > maxSim)
				{
					maxSim = sim;
					matchPoint1 = PointVector2[i];
					matchPoint2 = PointVector1[j];
				}
			}
			PointMATCH savePointPair;
			savePointPair.pointSrc = matchPoint1;
			savePointPair.pointDst = matchPoint2;
			savePointPair.similarity = maxSim;
			matchList.push_back(savePointPair);
		}
	}
}

void MatchPointGlobalFilter(vector<PointMATCH>&matchList, int imgW, int imgH)
{
	int thDx = imgW / 2;
	int thDy = imgH / 2;

	int* *hist2D = new int *[2 * thDy + 1];
	for (int i = 0; i < 2 * thDy + 1; i++)
	{
		hist2D[i] = new int[2 * thDx + 1];
	}

	for (int i = 0; i < 2 * thDy + 1; i++)//清0
	{
		for (int j = 0; j < 2 * thDx + 1; j++)
		{
			hist2D[i][j] = 0;
		}
	}

	for (int k = 0; k < matchList.size(); k++)//计算频次
	{
		int dx = matchList[k].pointSrc.x - matchList[k].pointDst.x;
		int dy = matchList[k].pointSrc.y - matchList[k].pointDst.y;
		if (dx<-thDx || dx>thDx || dy<-thDy || dy>thDy)
		{
			continue;
		}
		hist2D[dy + thDy][dx + thDx]++;// + thDy thDx应对负值 方便之后统计  -此处统计成直方图
		//cout << dy + thDy << " " << dx + thDx << endl;
	}

	int maxHist = 0;
	int gDx = 0, gDy = 0;
	int nearDist = 3;

	for (int i = nearDist; i < (2 * thDy + 1) - nearDist; i++)//以偏移值的7*7邻域累积个数和最大值为依据
	{
		for (int j = nearDist; j < (2 * thDx + 1) - nearDist; j++)
		{
			int sum = 0;
			for (int m = i - nearDist; m <= i + nearDist; m++)
			{
				for (int n = j - nearDist; n <= j + nearDist; n++)
				{
					sum += hist2D[m][n];
				}
			}
			if (sum > maxHist)
			{
				maxHist = sum;
				gDx = j;//
				gDy = i;
			}
		}
	}
	
	gDx = gDx - thDx;//下面距离位置的阈值范围 其中中心范围[nearDist,(width-nearDist)]   最大距离范围[2*nearDist, width]
	gDy = gDy - thDy;
	//cout <<"gDx: "<< gDx <<" "<<"gDy: "<< gDy << endl;
	for (int k = matchList.size() - 1; k >= 0; k--)//最大频次的最大距离范围-再拓展6*6容许范围
	{
		int dx = matchList[k].pointSrc.x - matchList[k].pointDst.x;
		int dy = matchList[k].pointSrc.y - matchList[k].pointDst.y;

		if (abs(dx - gDx) > 1 * nearDist || abs(dy - gDy) > 1 * nearDist)
		{
			matchList.erase(matchList.begin() + k);
		}
	}

	for (int i = 0; i < 2 * thDy + 1; i++)
	{
		delete[] hist2D[i];
	}
	delete[] hist2D;
}

void MatchPointGlobalFilterOffset(vector<PointMATCH>&matchList, int imgW, int imgH, int &Xoffset, int &Yoffset)
{
	int thDx = imgW / 2;
	int thDy = imgH / 2;

	int* *hist2D = new int *[2 * thDy + 1];
	for (int i = 0; i < 2 * thDy + 1; i++)
	{
		hist2D[i] = new int[2 * thDx + 1];
	}

	for (int i = 0; i < 2 * thDy + 1; i++)//清0
	{
		for (int j = 0; j < 2 * thDx + 1; j++)
		{
			hist2D[i][j] = 0;
		}
	}

	for (int k = 0; k < matchList.size(); k++)//计算频次
	{
		int dx = matchList[k].pointSrc.x - matchList[k].pointDst.x;
		int dy = matchList[k].pointSrc.y - matchList[k].pointDst.y;
		if (dx<-thDx || dx>thDx || dy<-thDy || dy>thDy)
		{
			continue;
		}
		hist2D[dy + thDy][dx + thDx]++;// + thDy thDx应对负值 方便之后统计
	}

	int maxHist = 0;
	int gDx = 0, gDy = 0;
	int nearDist = 3;

	for (int i = nearDist; i < (2 * thDy + 1) - nearDist; i++)//以偏移值的7*7邻域累积个数和最大值为依据
	{
		for (int j = nearDist; j < (2 * thDx + 1) - nearDist; j++)
		{
			int sum = 0;
			for (int m = i - nearDist; m <= i + nearDist; m++)
			{
				for (int n = j - nearDist; n <= j + nearDist; n++)
				{
					sum += hist2D[m][n];
				}
			}
			if (sum > maxHist)
			{
				maxHist = sum;
				gDx = j;//
				gDy = i;
			}
		}
	}

	gDx = gDx - thDx;//下面距离位置的阈值范围 其中中心范围[nearDist,(width-nearDist)]   最大距离范围[2*nearDist, width]
	gDy = gDy - thDy;

	Xoffset = gDx;
	Yoffset = gDy;
	//cout <<"gDx: "<< gDx <<" "<<"gDy: "<< gDy << endl;
	for (int k = matchList.size() - 1; k >= 0; k--)//最大频次的最大距离范围-再拓展6*6容许范围
	{
		int dx = matchList[k].pointSrc.x - matchList[k].pointDst.x;
		int dy = matchList[k].pointSrc.y - matchList[k].pointDst.y;

		if (abs(dx - gDx) > 2 * nearDist || abs(dy - gDy) > 2 * nearDist)
		{
			matchList.erase(matchList.begin() + k);
		}
	}

	for (int i = 0; i < 2 * thDy + 1; i++)
	{
		delete[] hist2D[i];
	}
	delete[] hist2D;
}

void featurePointAndDescriptorExtract(Mat& srcImg, vector<Point>& finalFeaturePoints, vector<vector<float>>& FeatureDescriptor)
{
	Mat srcImgProcess = Mat(srcImg.rows, srcImg.cols, CV_8UC1);
	ImgPreprocess(srcImg, srcImgProcess);

	Mat srcImgMCM = Mat(srcImgProcess.rows, srcImgProcess.cols, CV_8UC1);
	MinimunConvMap(srcImgProcess, srcImgMCM);

	vector<Point> srcFeaturePoints;
	DetectFeaturePoint(srcImgMCM, srcFeaturePoints);

	//double t = getTickCount();
	Mat srcImgEnhance = Mat(srcImgProcess.rows, srcImgProcess.cols, CV_8UC1);
	ImgSegment(srcImgProcess, srcImgEnhance);
	//cout << (getTickCount() - t) * 1000 / getTickFrequency() <<"ms"<< endl;
	FeaturePointDescriptor(srcImgEnhance, srcFeaturePoints, finalFeaturePoints, FeatureDescriptor);

}


double matchMCP(Mat& srcImg, Mat& dstImg)
{
	vector<Point> PointVector1, PointVector2;
	vector<vector<float>>FeatureDescriptor1, FeatureDescriptor2;
	vector<PointMATCH> matchList;

	featurePointAndDescriptorExtract(srcImg, PointVector1, FeatureDescriptor1);
	featurePointAndDescriptorExtract(dstImg, PointVector2, FeatureDescriptor2);
	SearchCorrespondingPoint(PointVector1, FeatureDescriptor1, PointVector2, FeatureDescriptor2, matchList);

	//vector<PointMATCH>detectMatchList;
	//detectMatchList.assign(matchList.begin(), matchList.end());
	//cout << PointVector1.size() << " " << PointVector2.size() << " " << matchList.size() << " ";

	int imgW = NORM_WIDTH;
	int imgH = NORM_HEIGHT;
//    cout << matchList.size() << endl;
	MatchPointGlobalFilter(matchList, imgW, imgH);
//    cout << FeatureDescriptor1.size() << endl;
//    cout << FeatureDescriptor2.size() << endl;
//    cout << PointVector1.size() << endl;
//    cout << PointVector2.size() << endl;
//    cout << matchList.size() << endl;
	//以下需释放内存--SearchCorrespondingPoint中
//    cout << FeatureDescriptor1[0] << endl;
//	for (int i = 0; i < FeatureDescriptor1.size(); i++)//释放FeaturePointDescriptor中描述子内存
//	{
//		delete[] FeatureDescriptor1.at(i);
//	}
//
//	for (int i = 0; i < FeatureDescriptor2.size(); i++)//释放FeaturePointDescriptor中描述子内存
//	{
//		delete[] FeatureDescriptor2.at(i);
//	}

	return double(2 * matchList.size()) / (FeatureDescriptor1.size() + FeatureDescriptor2.size()); //匹配率
	//return matchList.size();
}



