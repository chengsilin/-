#include<stdio.h>
#include<pthread.h>

#define N 10

int A[N] = {1,2,3,4,5,6,7,8,9,10};
int B[N] = {10000,20000,30000,40000,50000,60000,70000,80000,90000,100000};
int sum = 0;
pthread_t tid[N];


void* thr_fn(int arg)
{
    sum += A[arg] ;
	sum += B[arg] ;
    printf("Pthread %d: %d = A[%d] + B[%d]\n",arg,A[arg] + B[arg],arg,arg);
    return((void*)0);
}

int main(void)
{
    int i,j;
    int ret[N];
    for(i=0;i<N;i++)
    {
        ret[i] = pthread_create(&tid[i],NULL,thr_fn,i);
        if(ret[i]!=0)
        {
            printf("can't create thread!\n");
            exit(1);
        }
    }
    for(j=0;j<N;j++)
    {
        pthread_join(tid[j],NULL);
    }
    printf("A is:");
    for(i=0;i<N;i++)
    {
        printf(" %d",A[i]);
    }
    printf("\n");
    printf("B is:");
    for(i=0;i<N;i++)
    {
        printf(" %d",B[i]);
    }
    printf("\n");
    printf("the result is:%d",sum);
    printf("\n");

    sleep(2);
    exit(0);
}
