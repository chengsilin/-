#include<stdio.h>
#include <omp.h>
#include <time.h>
int vector_a[10] = {1,2,3,4,5,6,7,8,9,10};
int vector_b[10] = {1,2,3,4,5,6,7,8,9,10};
int vector_result[10];

int main(void)
{
    int i;
    time_t start_1,end_1,start_2,end_2;
    start_1 =time(NULL);

#pragma omp parallel for
    for(i=0;i<10;i++)
    {
        vector_result[i] = vector_a[i] + vector_b[i];
    }

    end_1 = time(NULL);

    start_2 = time(NULL);
    for(i=0;i<10;i++)
    {
        vector_result[i] = vector_a[i] + vector_b[i];
    }
    end_2 = time(NULL);
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

    printf("OpenMP cost time: %f ms\n",(double)(end_1-start_1));
    printf("Serial cost time: %f ms\n",(double)(end_2-start_2));


}
