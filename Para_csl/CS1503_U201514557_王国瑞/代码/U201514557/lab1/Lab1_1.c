#include<stdio.h>
#include<pthread.h>

int vector_a[10] = {1,2,3,4,5,6,7,8,9,10};
int vector_b[10] = {1,2,3,4,5,6,7,8,9,10};
int vector_result[10];
pthread_t tid[10];


void* thr_fn(int arg)
{
    vector_result[arg] = vector_a[arg] + vector_b[arg];
    printf("Pthread %d: vector_result[%d] = vector_a[%d] + vector_b[%d]\n",arg,arg,arg,arg);
    return((void*)0);
}

int main(void)
{
    int i,j;
    int ret[10];
    for(i=0;i<10;i++)
    {
        ret[i] = pthread_create(&tid[i],NULL,thr_fn,i);
        if(ret[i]!=0)
        {
            printf("can't create thread!\n");
            exit(1);
        }
    }
    for(j=0;j<10;j++)
    {
        pthread_join(tid[j],NULL);
    }
    printf("vector_a is:");
    for(i=0;i<10;i++)
    {
        printf(" %d",vector_a[i]);
    }
    printf("\n");
    printf("vector_b is:");
    for(i=0;i<10;i++)
    {
        printf(" %d",vector_b[i]);
    }
    printf("\n");
    printf("vector_result is:");
    for(i=0;i<10;i++)
    {
        printf(" %d",vector_result[i]);
    }
    printf("\n");

    sleep(2);
    exit(0);
}
