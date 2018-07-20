#include <stdio.h>
#include <mpi.h>
#include <math.h>  
int vector_a[10];
int vector_b[10];
int vector_c[10];
int main(int argc ,char *argv[])
{   
    int i;
    for(i=0;i<10;i++)
    {
      
         vector_a[i]=i;
         vector_b[i]=i;  
       
    }

    int rank, size;
    MPI_Init(&argc, &argv);  
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  //进程标识
    MPI_Comm_size(MPI_COMM_WORLD, &size);     //进程总数
    MPI_Status status;

    if(rank==0)
    {
       int i;
       for(i=1;i<size;i++)
       {
          MPI_Send(&vector_a[2*i-2],2,MPI_INT,i,0,MPI_COMM_WORLD);
       }
       for(i=1;i<size;i++)
       {
          MPI_Send(&vector_b[2*i-2],2,MPI_INT,i,1,MPI_COMM_WORLD);
       }
       for(i=1;i<size;i++)
       {
          MPI_Recv(&vector_c[2*i-2],2,MPI_INT,i,2,MPI_COMM_WORLD,&status);
          printf("vector_c[%d]= %d , vector_c[%d]= %d \n",2*i-2,vector_c[2*i-2],2*i-1,vector_c[2*i-1]);
       }
       
    }
    else
    {
       int A[2],B[2],C[2];
       MPI_Recv(A,2,MPI_INT,0,0,MPI_COMM_WORLD,&status);
       MPI_Recv(B,2,MPI_INT,0,1,MPI_COMM_WORLD,&status);
       int j;
       for(j=0;j<2;j++)
       {
          C[j]=A[j]+B[j];
       }
       MPI_Send(C,2,MPI_INT,0,2,MPI_COMM_WORLD);
    }
    
    MPI_Finalize();

    return 0;
}










