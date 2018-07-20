//nvcc -arch=sm_10 -o parallel_cuda parallel_cuda.cu `pkg-config --libs --cflags cudaopencv`
#include <iostream>
//#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv/cv.h>
//#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <cuda.h>
#include "cuda_runtime.h"  
#include "device_launch_parameters.h" 

using namespace cv;
using namespace std;

double get_time()//获取当前时间
{
	double time[2];	
	struct timeval time_tmp;
	gettimeofday(&time_tmp, NULL);
	time[0]=time_tmp.tv_sec;//秒数
	time[1]=time_tmp.tv_usec;//微秒数
	return time[0]+time[1]*1.0e-6;
}

__global__ void parallel_cuda(uchar *dev_src,uchar *dev_dst,int row,int col,int NUM,int channel)
{
	int thread_id1 = (blockIdx.x * gridDim.x + blockIdx.y) * blockDim.x * blockDim.y + threadIdx.x * blockDim.x + threadIdx.y;
	int block_row = row / NUM + 1;
	int i, j, row_start, row_end;
	if(thread_id1 < NUM){
		row_start = thread_id1 * block_row + 1;
		row_end = row_start + block_row;
		
		for(i = row_start; (i < row_end)&&(i<row-1) ;i++){
			const uchar *pre = dev_src+(i-1)*channel*col;
			const uchar *cur = dev_src+(i)*channel*col;
			const uchar *next = dev_src+(i+1)*channel*col;
			uchar *outData = dev_dst+(i)*channel*col;
			for (j = channel; j < (col-1)*channel; j++) {
				int tmp = ( cur[j]*9 - pre[j- channel]- pre[j]- pre[j + channel]-cur[j- channel]-cur[j + channel] -next[j- channel]-next[j]-next[j + channel]);
				outData[j] = tmp;
			}
		}
	}
}

int main(void)
{
	Mat imagein = imread("lena.jpg");
	Mat imageout;
	imageout.create(imagein.size(),imagein.type());
	if( imagein.empty()==true)
	{
		printf( "Can't open file!\n " );
		return -1;
	}
	const int channel = imagein.channels();

	double time1, time2;
	time1 = get_time();
	dim3 grid(1, 1, 1);
	dim3 block(8, 8, 1);
	int thread_num = 8*8;
	int IMG_SIZE = sizeof(uchar)*imagein.rows * channel * imagein.cols;
	uchar *dev_dst;
	uchar *dev_src;
	cudaError_t error = cudaSuccess;
    error = cudaMalloc((void **)&dev_dst, IMG_SIZE);
    error = cudaMalloc((void **)&dev_src, IMG_SIZE);//GPU内空间分配
	if (error != cudaSuccess) {
        printf("Fail to cudaMalloc on GPU");
        return 1;
    }
	cudaMemcpy(dev_src, imagein.ptr<uchar>(0), IMG_SIZE, cudaMemcpyHostToDevice);//简单的数据转移
	parallel_cuda<<<grid, block>>>(dev_src,dev_dst,imagein.rows,imagein.cols,thread_num,channel);//对每一项计算
	cudaThreadSynchronize();
	cudaMemcpy(imageout.ptr<uchar>(0), dev_dst, IMG_SIZE, cudaMemcpyDeviceToHost);
	time2 =get_time();
	double runTime = (double)(time2 - time1);
	printf("Image convolution time in cuda: %lf \n",runTime/300);
    IplImage tmp = IplImage(imageout);
    CvArr* arr = (CvArr*)&tmp;	
	cvSaveImage("output.jpg" , arr);
	cudaFree(dev_src);
	cudaFree(dev_dst);
	return 0;
}
