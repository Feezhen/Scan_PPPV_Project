#ifndef _POINT_FEATURE_H_
#define _POINT_FEATURE_H_

#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

#define FEATURE_HOG_DIM 4*4*9
#define FEATURE_HVD_DIM 9*5
#define FEATURE_SIFT_DIM 128
#define FEATURE_POINT_DIM FEATURE_HOG_DIM

typedef struct featurePointMatch{
	Point pointSrc;
	Point pointDst;
	float similarity;
} PointMATCH;

double matchMCP(Mat& srcImg, Mat& dstImg);
void featurePointAndDescriptorExtract(Mat& srcImg, vector<Point>& finalFeaturePoints, vector<vector<float>>& FeatureDescriptor);
void DetectFeaturePoint(Mat MCMImg, vector<Point>&featurePoints);
void FeatureExtractHVD(Mat srcImg, int cellRows, int cellCols, float* featArray);
void featureExtractHog(const Mat &orgImg, vector<float> &featureArray);
void FeaturePointDescriptor(Mat VPMImg, vector<Point> featurePoints, vector<Point> &getFeaturePoints, vector<vector<float>>& FeatureDescriptor);
float calCosineSimilarity(const vector<float>& feature1, const vector<float>& feature2, int length);
void SearchCorrespondingPoint(vector<Point>& PointVector1, vector<vector<float>>& FeatureDescriptor1, vector<Point>& PointVector2, vector<vector<float>>& FeatureDescriptor2, vector<PointMATCH>& matchList);
void MatchPointGlobalFilter(vector<PointMATCH>&matchList, int imgW, int imgH);

#endif
