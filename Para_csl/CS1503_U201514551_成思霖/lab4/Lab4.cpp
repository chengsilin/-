#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/time.h>

using namespace cv;
const int kernel[3][3] = {0,-1,0,-1,5,-1,0,-1,0 };//卷积核矩阵

double get_time();//获取当前时间

int main(){
	struct  timeval start, end;
	float time_use;

	MPI_Init(NULL, NULL);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int *begin_row = NULL;
	int *end_row = NULL;
	Mat img;
	if(rank == 0){
		gettimeofday(&start, NULL);
		img = imread("lena.jpg");
		begin_row = (int *)malloc(sizeof(int) * size);//每个节点处理的最小行序号
		end_row = (int *)malloc(sizeof(int) * size);//每个节点处理的最大行序号
		
		int row = img.rows - 2;//总共需要处理的行数
		int col = img.cols;//列数
		int nchannels = img.channels();//通道数
		int info[2];
		info[0] = col;
		info[1] = nchannels;
		dRow = row/size;//将图像分块处理
		if(dRow*size != row) dRow++;
		
		int r = 1; 
		for(int i = 0; i < size; i++){
			//计算每个节点处理的最小行序号和最大行序号
			if(i == size - 1){
				begin_row[i] = r - 1;
				end_row[i] = img.rows - 1;
			}
			else{
				begin_row[i] = r - 1;
				end_row[i] = r + dRow;
			}
			r += dRow;
			//将节点分到的像素点分到对应的节点
			if(i != 0){
				uchar *send_buf = (uchar *)malloc(sizeof(uchar) * (end_row[i] - begin_row[i] + 1) * col * nchannels);
				int n = 0;
				for(int j = begin_row[i]; j <= end_row[i]; j++){
					uchar *rowPointer = img.ptr<uchar>(j);
					for(int k = 0; k < col * nchannels; k++){
						send_buf[n++] = rowPointer[k];
					}
				}
				MPI_Send(send_buf, n, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
				MPI_Send(info, 2, MPI_INT, i, 1, MPI_COMM_WORLD);
				//printf("send to node%d over\n", i);
				free(send_buf);
			}
		}
	}
	if(rank != 0){
		int rec_num;//像素点的个数
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &rec_num);
		uchar *rec_buf = (uchar *)malloc(sizeof(uchar) * rec_num);//接收缓冲
		MPI_Recv(rec_buf, rec_num, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int *info = (int *)malloc(2 * sizeof(int));//列数和通道数
		MPI_Recv(info, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int row_num = rec_num / info[0] / info[1];//行数
		int n = 0;
		int col = info[0];
		int channel = info[1];
		uchar *send_buf = (uchar *)malloc(sizeof(uchar) * (rec_num - 2 * col * channel));
		for (int i = 1; i < row_num - 1; i++) 
		{
			int startCol = channel;//每一行的开始处理点
			int endCol = (col-1)* channel;//每一行的处理结束点
			for(int j=startCol; j < endCol; j++) 
			//输出图像的遍历指针与当前行的指针同步递增, 以每行的每一个像素点的每一个通道值为一个递增量, 因为要考虑到图像的通道数
			{
				const uchar up = rec_buf[(i - 1) * col * channel + j];
				const uchar down = rec_buf[(i + 1) * col * channel + j];
				const uchar mid = rec_buf[i * col * channel + j];
				const uchar left = rec_buf[i * col * channel + j - channel];
				const uchar right = rec_buf[i * col * channel + j + channel];
				send_buf[n++] = saturate_cast<uchar>(5 * mid - up - down - left - right); //saturate_cast<uchar>保证结果在uchar范围内
			}
		}
		MPI_Send(send_buf, n, MPI_UNSIGNED_CHAR, 0, rank + 1, MPI_COMM_WORLD);
	}
	if(rank == 0){
		Mat newImg;
		newImg.create(img.size(), img.type());
		int col = img.cols;
		int nchannels = img.channels();
		for(int i = begin_row[0] + 1; i < end_row[0]; i++){
			for(int j = 1; j < col - 1; j ++){
				for(int k = 0; k < nchannels; k++){
					const uchar up = img.at<Vec3b>(i-1, j)[k];
					const uchar down = img.at<Vec3b>(i+1, j)[k];
					const uchar mid = img.at<Vec3b>(i, j)[k];
					const uchar left = img.at<Vec3b>(i, j-1)[k];
					const uchar right = img.at<Vec3b>(i, j+1)[k];
					newImg.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(5 * mid - up - down - left - right);
				}
			}
		}
		for(int i = 1; i < size; i++){
			int rec_num;//像素点的个数
			MPI_Status status;
			MPI_Probe(i, i + 1, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &rec_num);
			uchar *rec_buf = (uchar *)malloc(sizeof(uchar) * rec_num);//接收缓冲
			MPI_Recv(rec_buf, rec_num, MPI_UNSIGNED_CHAR, i, i + 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int n = 0;
			for(int j = begin_row[i] + 1; j < end_row[i]; j++){
				for(int k = 0; k < col; k++){
					for(int h = 0; h < nchannels; h++){
						newImg.at<Vec3b>(j, k)[h] = rec_buf[n++];
					}
				}
			}
		}
		newImg.row(0).setTo(Scalar(0));
		newImg.row(newImg.rows-1).setTo(Scalar(0));
		newImg.col(0).setTo(Scalar(0));
		newImg.col(newImg.cols-1).setTo(Scalar(0));
		imwrite("output.jpg", newImg);
		gettimeofday(&end, NULL);
		time_use=(end.tv_sec-start.tv_sec)*1000+(end.tv_usec-start.tv_usec)/1000;
		printf("OpenMPI runtime: %.6f ms\n", time_use);
	}
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}

