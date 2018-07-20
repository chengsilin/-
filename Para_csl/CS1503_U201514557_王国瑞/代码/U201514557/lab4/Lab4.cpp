#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cv.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>
#include <highgui.h>

using namespace cv;
double get_time();//获取当前时间

int main(int argc, char* argv[])
{
	const char* image = "lena.jpg";
	Mat imgin = imread(image);
	Mat imgout;
	imgout.create(imgin.size(), imgin.type());
	if( imgin.empty() ){
		printf("Can't open file!\n");
		return -1;
	}

	int row = imgin.rows;//行数
	int column = imgin.cols;//列数
	int channel = imgin.channels();//通道数
	double time1, time2;

	int num, id;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&num);//获取进程总数
	MPI_Comm_rank(MPI_COMM_WORLD,&id);//获取当前进程标志
	int brow = row / num;//将图像分块处理
    if(brow*num != row) brow++;

	int i, j, row_start, row_end;
	if(id != 0 && id != num-1)//第一个进程用于收集数据
	{
		row_start = (id-1)*brow + 1;
		row_end = row_start + brow;
		for(i = row_start; i < row_end; i++){//获取数据源阵列
			const uchar* pre = imgin.ptr<uchar>(i-1);
			const uchar* cur = imgin.ptr<uchar>(i);
			const uchar* next = imgin.ptr<uchar>(i+1);
			uchar* outdata = imgout.ptr<uchar>(i);
			for(j = channel; j < (column-1)*channel; j++){//像素点锐化
				outdata[j] = saturate_cast<uchar>( cur[j]*9 - pre[j-channel] - pre[j] - pre[j+channel] - cur[j-channel] - cur[j+channel] - next[j-channel] - next[j] - next[j+channel]);
			}
		}
		uchar* buf_send = imgout.ptr<uchar>(row_start);
		MPI_Send(buf_send, brow*column*channel, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		time1 = get_time();
		for(i = 1; i < num-1; i++)
		{
			uchar* buf_recv = imgout.ptr<uchar>((i-1)*brow + 1);
			MPI_Recv(buf_recv, brow*column*channel, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		uchar* buf_recv = imgout.ptr<uchar>((num-2)*brow + 1);
		row_start = (num-2)*brow + 1;
		row_end = row - 1;
		MPI_Recv(buf_recv, (row_end-row_start)*column*channel, MPI_CHAR, num-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		time2 = get_time();
		printf("Image convolution time in MPI: %lf\n",(double)(time2-time1));
        IplImage tmp = IplImage(imgout);
        CvArr* arr = (CvArr*)&tmp;
		cvSaveImage("output.jpg", arr);
	}
	MPI_Finalize();
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
