#include <iostream>
#include <queue>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>
using namespace std;

#define MAX_QUEUE_SIZE 5

int N;
int adjacency[105][105]={0};

double get_time()//获取当前时间
{
	double time[2];
	struct timeval time_tmp;
	gettimeofday(&time_tmp, NULL);
	time[0]=time_tmp.tv_sec;//秒数
	time[1]=time_tmp.tv_usec;//微秒数
	return time[0]+time[1]*1.0e-6;
}
int myRand(int row, int col)
{
	if(row==col)
        return 0;
	int temp=rand()%4;
	if(temp<3)
		return 1;
	else
		return 0;
}

int main(int argc, char *argv[])
{
	//Variables and Initializations
	int size, rank;
	//MPI Code
	double start = get_time();
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0)
	{
		cin>>N;
        int i,j,rands;
		for(i=0; i<N; i++)
			for(j=i;j<N; j++)
			{
				rands=myRand(i,j);
				adjacency[i][j]=rands;
				adjacency[j][i]=rands;
			}
		
	}

	if(rank != 0){
		queue<int> vertex;;
		int flag[N]={0};
		vertex.push(start);
		flag[start]=1;
		int head,i   ;
		while(!vertex.empty())
		{
			head=vertex.front();
			vertex.pop();
			#pragma omp parallel for
			for(i=0; i<N; i++)
			{
				if(adjacency[head][i]==1&&flag[i]==0)
				{
                flag[i]=1;
                vertex.push(i);
				}
			}
			MPI_Send(&isNeed, 1,MPI_INT,i+1, 0, MPI_COMM_WORLD);
		}
		
		cout<<endl;
	}
	
	//End of BFS code
	MPI_Finalize();
	double stop = get_time();
	double Parallel_runTime = stop - start;
	cout << "time in OPENMP:" << Parallel_runTime  << endl;
	return 0;
}