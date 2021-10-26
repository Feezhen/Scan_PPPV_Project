#include "gabor.h"

//卷积核生成函数
Mat gabor_filter(float theta)
{
	//Mat p0 = Mat::zeros(2, 1, CV_8U);//translation coordinates
	//int bw = 8;//bandwidth
	//int lambda = 50;//wavelength of cosinusoidal part of the filter (pixel)   30
	//int gamma = 20;//Aspect ratio(sigma_y * sigma_x)  6
	//int size = 45;//Size of the kernel of the filter  30
	////dark param: 8,30,6,45

	Mat p0 = Mat::zeros(2, 1, CV_8U);//translation coordinates
	int bw = 8;//bandwidth
	int lambda = 30;//wavelength of cosinusoidal part of the filter (pixel)   30
	int gamma = 40;//Aspect ratio(sigma_y * sigma_x)  6
	int size = 75;//Size of the kernel of the filter  30

	float sigma = (lambda / CV_PI) * sqrt(log(2) / 2) * (pow(2,bw) + 1) / (pow(2, bw) - 1);
	float sigma_x = sigma;
	float sigma_y = sigma / gamma;
	Mat wm = Mat::zeros(2, 1, CV_32F);
	wm.at<float>(0, 0) = 2 * CV_PI / lambda;//spatial modulation frequency

	if (size % 2 == 0)
		size += 1;
	
	Mat x = Mat::zeros(size, size, CV_8S);
	Mat y = Mat::zeros(size, size, CV_8S);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			x.at<char>(i, j) = j - size / 2;
			y.at<char>(i, j) = size / 2 - i;
		}
	}

	Mat x0 = Mat::ones(size, size, CV_8U);
	Mat y0 = Mat::ones(size, size, CV_8U);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			x0.at<uchar>(i, j) = x0.at<uchar>(i, j) * p0.at<uchar>(0, 0);
			y0.at<uchar>(i, j) = y0.at<uchar>(i, j) * p0.at<uchar>(1, 0);
		}
	}

	Mat x_theta = Mat::ones(size, size, CV_32F);
	Mat y_theta = Mat::ones(size, size, CV_32F);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			x_theta.at<float>(i, j) = x.at<char>(i, j) * cos(theta) + y.at<char>(i, j) * sin(theta);
			y_theta.at<float>(i, j) = - x.at<char>(i, j) * sin(theta) + y.at<char>(i, j) * cos(theta);
		}
	}

	Mat x0_theta = Mat::ones(size, size, CV_32F);
	Mat y0_theta = Mat::ones(size, size, CV_32F);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			x0_theta.at<float>(i, j) = x0.at<uchar>(i, j) * cos(theta) + y0.at<uchar>(i, j) * sin(theta);
			y0_theta.at<float>(i, j) = -x0.at<uchar>(i, j) * sin(theta) + y0.at<uchar>(i, j) * cos(theta);
		}
	}

	x_theta = x_theta - x0_theta;
	y_theta = y_theta - y0_theta;

	Mat gauss_part = Mat::ones(size, size, CV_32F);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			gauss_part.at<float>(i, j) = exp(-0.5 * (pow(x_theta.at<float>(i, j),2) / pow(sigma_x, 2) + pow(y_theta.at<float>(i, j), 2) / pow(sigma_y, 2)));
		}
	}

	Mat psi = Mat::ones(size, size, CV_32F);
	Mat cos_part = Mat::ones(size, size, CV_32F);
	Mat gb = Mat::ones(size, size, CV_32F);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			psi.at<float>(i, j) = -wm.at<float>(0, 0) * x0_theta.at<float>(i, j);
			cos_part.at<float>(i, j) = cos(wm.at<float>(0, 0) * x_theta.at<float>(i, j) + psi.at<float>(i, j));
			gb.at<float>(i, j) = gauss_part.at<float>(i, j) * cos_part.at<float>(i, j);
		}
	}

	Scalar tempVal = mean(gb);
	float m = tempVal.val[0];

	Mat gabor_kernel = Mat::zeros(size, size, CV_32F);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			gabor_kernel.at<float>(i, j) = gb.at<float>(i, j) - m;
		}
	}

	return gabor_kernel;
}

//滤波函数
Mat gabor(Mat src)
{
	/*******************
	src:单通道
	conv_img:channel通道
	out_img:单通道
	*******************/
	int channel = 16;
	Mat conv_img = Mat::zeros(src.rows, src.cols, CV_32FC(channel));//自定义通道数
	typedef Vec<float, 16> Vec_F;//改通道数的化这里也要改维度
	Mat tmp;
	Mat out_img = Mat(src.rows, src.cols, CV_32F);

	Point anchor = Point(-1, -1);
	if (src.channels() == 3) //RGB convert to GRAY
	{
		cvtColor(src, src, CV_RGB2GRAY);
	}
	src.convertTo(src, CV_32F);

	//GaussianBlur(src, src, Size(3, 3), 2, 0); //高斯函数滤波，没必要

	for (int i = 0; i < channel; i++)
	{
		float theta = i * (CV_PI / channel);
		Mat gabor_kernel = gabor_filter(theta);
		filter2D(src, tmp, -1, gabor_kernel, anchor, 0, BORDER_REFLECT); //滤波
		for (int m = 0; m < src.rows; m++)
		{
			for (int n = 0; n < src.cols; n++)
			{
				conv_img.at<Vec_F>(m, n)[i] = tmp.at<float>(m, n);
			}
		}
	}

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			out_img.at<float>(i, j) = conv_img.at<Vec_F>(i, j)[0];
			for (int c = 1; c < channel; c++)
			{
				if (out_img.at<float>(i, j) > conv_img.at<Vec_F>(i, j)[c])  //取16个通道中的最小值
					out_img.at<float>(i, j) = conv_img.at<Vec_F>(i, j)[c];
			}
		}
	}
	/*imshow("out_img1", out_img);
	waitKey();*/
	/*dilate(out_img, out_img, Mat(), Point(-1, -1), 3);
	erode(out_img, out_img, Mat(), Point(-1, -1), 3);
	imshow("out_img2", out_img);
	waitKey();*/

	double max, min;
	cv::Point min_loc, max_loc;
	cv::minMaxLoc(out_img, &min, &max, &min_loc, &max_loc);
	/*cout << out_img.at<float>(0, 0) << endl;*/
	/*cout << max << endl << min << endl;
	out_img.at<float>(min_loc.y, min_loc.x) = 0;
	cv::minMaxLoc(out_img, &min, &max, &min_loc, &max_loc);
	cout << max << endl << min << endl;*/
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			//cout << out_img.at<float>(i, j) << endl;
			out_img.at<float>(i, j) = ((out_img.at<float>(i, j) - min) / (max - min)) * 255;
			/*if (out_img.at<float>(i, j) <= 10)
			{
				out_img.at<float>(i, j) = 0;
			}
			else
			{
				out_img.at<float>(i, j) = 1;
			}*/
		}
	}
	/*imshow("out_img", out_img);
	waitKey();*/

	return out_img;
}