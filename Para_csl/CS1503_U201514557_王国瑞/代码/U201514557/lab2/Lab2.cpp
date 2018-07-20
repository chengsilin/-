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


using namespace cv;
using namespace std;

const int kernel[3][3] = {-1,-1,-1,-1,9,-1,-1,-1,-1 };//卷积核矩阵
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
    imageout.create(imagein.size(), imagein.type());
    IplImage tmp = IplImage(imageout);
    CvArr* arr = (CvArr*)&tmp;
	int row = imagein.rows;//行数
	int column = imagein.cols;//列数
	int channel = imagein.channels();//通道数
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
	int channel = imagein.channels();
	int columnt = imagein.cols;
	for(i = row_start; (i < row_end)&&(i < imagein.rows - 1) ;i++){
		const uchar*pre = imagein.ptr<uchar>(i-1);
		const uchar*cur = imagein.ptr<uchar>(i);
		const uchar*next = imagein.ptr<uchar>(i+1);
		uchar*outData = imageout.ptr<uchar>(i);
		for (j = channel; j < (columnt-1)*channel; j++) {
			outData[j] = saturate_cast<uchar>( cur[j]*9 - pre[j- channel]- pre[j]- pre[j + channel]-cur[j- channel]-cur[j + channel] -next[j- channel]-next[j]-next[j + channel]);
		}
	}
}
