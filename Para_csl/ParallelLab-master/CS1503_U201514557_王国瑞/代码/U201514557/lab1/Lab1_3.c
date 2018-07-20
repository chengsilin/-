#include<stdio.h>
#include<mpi.h>

int vector_a[10]={1,2,3,4,5,6,7,8,9,10};
int vector_b[10]={1,2,3,4,5,6,7,8,9,10};
int vector_result[10];

int main (int argc ,char**argv)
{
   int size=0;
   int rank=0;
   MPI_Status stat;
   MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

   if(rank==0)
  {
    int i ;
   for(i=1;i<size;i++)
   {
    MPI_Send (vector_a+2*i-2, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
   }

   for(i=1;i<size;i++)
   {
    MPI_Send (vector_b+2*i-2, 2, MPI_INT, i, 1, MPI_COMM_WORLD);
   }

 for(i=1;i<size;i++)
   {
     MPI_Recv (vector_result+2*i-2, 2, MPI_INT, i, 2, MPI_COMM_WORLD, &stat);
   }

  for(i=0;i<10;i++)
  {
    printf("%d  ",vector_result[i]);
  }
  }
  else
  {
     int a[2],b[2],c[2];

     MPI_Recv (a, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
     MPI_Recv (b, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat);
     c[0]=a[0]+b[0];
     c[1]=a[1]+b[1];
     MPI_Send (c, 2, MPI_INT, 0, 2, MPI_COMM_WORLD);
   printf("rank is %d  result0 is %d  result1 is %d a[0] is %d a[1] is %d \n",rank,c[0],c[1],a[0],a[1]);
  }

  MPI_Finalize ();






return 0;
}
