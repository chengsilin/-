#include "omp.h"
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
double start,stop;
Mat imgout;

double get_time();//获取当前时间
Mat imgConv(Mat imagein);//并行锐化函数
const int kernel[3][3] = {0,-1,0,-1,5,-1,0,-1,0 };//卷积核矩阵

int main(int argc, char* argv[])
{
	const char* imagename = "lena.jpg";
	Mat img = imread(imagename);
	if (img.empty())
	{
		fprintf(stderr, "Can't open file\n");
		return -1;
	}
	
	int row = img.rows;
	int column = img.cols;
	imgout.create(img.size(), img.type());

	imgout.row(0).setTo(Scalar(0));
	imgout.row(row - 1).setTo(Scalar(0));
	imgout.col(0).setTo(Scalar(0));
	imgout.col(column - 1).setTo(Scalar(0));

	//并行运行
	start = get_time();
    imgConv(img);
	stop = get_time();
	double Parallel_runTime = stop - start;
	cout << "Image convolution time in OpenMP:" << Parallel_runTime  << endl;
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

Mat imgConv(Mat imagein)
{
	int row = imagein.rows;
	int column = imagein.cols;
	int channel = imagein.channels();
#pragma omp parallel for
	for (int i = 1; i < row - 1; i++) {
		const uchar*pre = imagein.ptr<uchar>(i-1);  // 上一行数据的指针
		const uchar*cur = imagein.ptr<uchar>(i);//当前行，第i行
		const uchar*next = imagein.ptr<uchar>(i+1);//下一行
		uchar*outData = imgout.ptr<uchar>(i);//输出图像的第i行
		int startCol = channel;//每一行的开始处理点
        int endCol = (imagein.cols-1)* channel;//每一行的处理结束点
	// #pragma omp parallel for
        for(int j=startCol; j < endCol; j++) 
	   //输出图像的遍历指针与当前行的指针同步递增, 以每行的每一个像素点的每一个通道值为一个递增量, 因为要考虑到图像的通道数
		{
			outData[j] = saturate_cast<uchar>( 5*cur[j]-pre[j]-next[j]-cur[j-channel]-cur[j+channel]); //saturate_cast<uchar>保证结果在uchar范围内
		}
	}
	return imgout;
}

