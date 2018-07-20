#include <iostream>
#include <queue>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

int N;
int adjacency[105][105]={0};
/*
*功能：随机生成邻接矩阵，25%的概率两个顶点之间不存在一条边
*
*/
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
/*
*功能：随机生成邻接矩阵
*
*/
void initMap()
{
    int i,j,weight;
	cout<<"Please input the number of vertices:";
	cin>>N;
    for(i=0; i<N; i++)
        for(j=i; j<N; j++)
        {
            weight=myRand(i,j);
            adjacency[i][j]=weight;
            adjacency[j][i]=weight;
        }
	//调试状态下输出邻接矩阵
}
/*
*功能：宽度优先搜索
*
*/
void boradSearch(int start)
{
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
    }
	cout<<endl;
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

int main(int argc, char** argv)
{
    initMap();
	double start = get_time();
    boradSearch(0);//从顶点0开始遍历随机图
	double stop = get_time();
	double Parallel_runTime = stop - start;
	cout << "time in OPENMP:" << Parallel_runTime  << endl;
    return 0;
}
