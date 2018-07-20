#include <iostream>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>


using namespace cv;
using namespace std;

const int kernel[3][3] = {0,-1,0,-1,5,-1,0,-1,0 };//卷积核矩阵
Mat imagein;
Mat imageout;
int countt;
int core_num;

double get_time();//获取当前时间
void *parallel_pthread(void*);//并行线程处理

int main()
{
	int timer = 0;
	double time1,time2;

	const char* image = "lena.jpg";
    imagein = imread(image);
	int row = imagein.rows;//行数
	int column = imagein.cols;//列数
	int channel = imagein.channels();//通道数
    imageout.create(imagein.size(), imagein.type());
	imageout.row(0).setTo(Scalar(0));
	imageout.row(row - 1).setTo(Scalar(0));
	imageout.col(0).setTo(Scalar(0));
	imageout.col(column - 1).setTo(Scalar(0));
    IplImage tmp = IplImage(imageout);
    CvArr* arr = (CvArr*)&tmp;
	cout << "Input thread num:";
	cin >> core_num;

	countt = row/core_num;//将图像分块处理
    if(countt*core_num != row) countt++;
	int pth[core_num];
	pthread_t *pid;
	pid = (pthread_t *)malloc(sizeof(pthread_t)*(core_num));
	time1 = get_time();
	for(timer = 0; timer < 100; timer++)
	{
		for(int i = 0; i < core_num; i++){
			pth[i] = i;
			pthread_create(&pid[i],NULL,parallel_pthread,&pth[i]);
		}
		for(int i = 0; i < core_num; i++){
			pthread_join(pid[i],NULL);
		}
	}
	time2 = get_time();
	cout << "Image convolution time in pthread:"<<(double)(time2-time1) / 100 << endl;
    cvSaveImage("output.jpg", arr);
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

void *parallel_pthread(void* pp)
{
	int row_start = (*(int*)pp) * countt + 1;
	int row_end = row_start + countt;
	int i,j;
	int channel = imagein.channels();//通道个数
	for(i = row_start; (i < row_end)&&(i < imagein.rows - 1) ;i++)  // 第一行和最后一行无法计算
	{ 
		const uchar*pre = imagein.ptr<uchar>(i-1);  // 上一行数据的指针
		const uchar*cur = imagein.ptr<uchar>(i);//当前行，第i行
		const uchar*next = imagein.ptr<uchar>(i+1);//下一行
		uchar*outData = imageout.ptr<uchar>(i);//输出图像的第i行
		int startCol = channel;//每一行的开始处理点
        int endCol = (imagein.cols-1)* channel;//每一行的处理结束点
        for(j=startCol; j < endCol; j++) 
	   //输出图像的遍历指针与当前行的指针同步递增, 以每行的每一个像素点的每一个通道值为一个递增量, 因为要考虑到图像的通道数
		{
			outData[j] = saturate_cast<uchar>( 5*cur[j]-pre[j]-next[j]-cur[j-channel]-cur[j+channel]); //saturate_cast<uchar>保证结果在uchar范围内
		}
	}
}
