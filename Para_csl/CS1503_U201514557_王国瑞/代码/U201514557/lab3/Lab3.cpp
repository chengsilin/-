#include "omp.h"
//#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include<highgui.h>
#include <cv.h>

using namespace std;
using namespace cv;
double start,stop;

double get_time();//获取当前时间
Mat imgConv(Mat rp);//并行锐化函数
int kernel[3][3] = { -1, -1, -1, -1, 9, -1 ,-1, -1, -1 };//卷积核矩阵

int main(int argc, char* argv[])
{
	const char* imagename = "lena.jpg";
	Mat img = imread(imagename);
	if (img.empty())
	{
		fprintf(stderr, "Can't open file\n");
		return -1;
	}

	//并行运行
	start = get_time();
	for (int i = 0; i < 100; i++) {
		imgConv(img);
	}
	stop = get_time();
	double Parallel_runTime = stop - start;
	//运行100次
	cout << "Image convolution time in OpenMP:" << Parallel_runTime / 100 << endl;
    IplImage tmp = IplImage(imgConv(img));
    CvArr* arr = (CvArr*)&tmp;
	cvSaveImage("output.jpg",arr);
	return 0;
}

double get_time()//获取当前时间
{
	double time[2];
	struct timeval time_tmp;
	gettimeofday(&time_tmp, NULL);
	time[0]=time_tmp.tv_sec;//秒数
	time[1]=time_tmp.tv_usec;//微秒数
	return time[0]+time[1]*1.0e-6;
}

Mat imgConv(Mat rp)
{
	int row = rp.rows;
	int column = rp.cols;
	int channel = rp.channels();
	Mat imgout;
	imgout.create(rp.size(), rp.type());

	imgout.row(0).setTo(Scalar(0));
	imgout.row(row - 1).setTo(Scalar(0));
	imgout.col(0).setTo(Scalar(0));
	imgout.col(column - 1).setTo(Scalar(0));
#pragma omp parallel for
	for (int i = 1; i < row - 1; i++) {
		const uchar*pre = rp.ptr<uchar>(i - 1);
		const uchar*cur = rp.ptr<uchar>(i);
		const uchar*next = rp.ptr<uchar>(i + 1);
		uchar*outData = imgout.ptr<uchar>(i);
		for (int j = channel; j < (column - 1)*channel; j++) {
			outData[j] = saturate_cast<uchar>(
				pre[j - channel] * kernel[0][0] + pre[j] * kernel[0][1] + pre[j + channel] * kernel[0][2]
				+ cur[j - channel] * kernel[1][0] + cur[j] * kernel[1][1] + cur[j + channel] * kernel[1][2]
				+ next[j - channel] * kernel[2][0] + next[j] * kernel[2][1] + next[j + channel] * kernel[2][2]);
		}
	}
	return imgout;
}

