#include<stdio.h>
#include <omp.h>
#include <time.h>
#define Size 100000000
int vector_a[Size];
int vector_b[Size];
int vector_result[Size];

int main(void)
{
    int i;
    time_t start_1,end_1,start_2,end_2;
    for(i=0;i<Size;i++)
    {
        vector_a[i] = i;
        vector_b[i] = i;
    }
    start_1 =time(NULL);
#pragma omp parallel for
    for(i=0;i<Size;i++)
    {
        vector_result[i] = vector_a[i] + vector_b[i];
    }

    end_1 = time(NULL);

    start_2 = time(NULL);
    for(i=0;i<Size;i++)
    {
        vector_result[i] = vector_a[i] + vector_b[i];
    }
    end_2 = time(NULL);
/*    printf("vector_a is:");
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
    */

    printf("OpenMP cost time: %f ms\n",(double)(end_1-start_1));
    printf("Serial cost time: %f ms\n",(double)(end_2-start_2));


}
