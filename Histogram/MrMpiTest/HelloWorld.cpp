#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#include <mapreduce.h>
#include <keyvalue.h>


using MAPREDUCE_NS::KeyValue;
using MAPREDUCE_NS::MapReduce;


void foo(int itask, KeyValue *, void* data)
{
  printf("data update %c %c in task %d\n", ((char *)data)[0], ((char *)data)[1], itask);
}


int main (int argc, char *argv[])
{
  int rank, size;
  char data[21];
  data[20] = 0;
  printf("Got here!\n");
  MPI_Init (&argc, &argv); /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank); /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size); /* get number of processes */
  printf( "Hello world from process %d of %d\n", rank, size );
  
  MPI_Barrier(MPI_COMM_WORLD);
  if (true)//rank == 0)
  {
    FILE * f = fopen("foo", "r");
    fread(data, 20, 1, f);
    
    printf("BTW data is %s %d\n", data, sizeof(data));
    
    for (int i = 1; i < 4; ++ i)
      MPI_Send(data, sizeof(data), MPI_CHAR, i, 1234, MPI_COMM_WORLD);
  }
  else
  {
    MPI_Status status;
    
    MPI_Recv(data, sizeof(data), MPI_CHAR, 0, 1234, MPI_COMM_WORLD, & status);
  }
  printf( "Goodbye cruel world :( from process %d of %d\n", rank, size );
  MPI_Barrier(MPI_COMM_WORLD);
  
  MapReduce* mr = new MapReduce(MPI_COMM_WORLD);
  for (int i = 0; i < 10; ++ i)
    mr->map(4, foo, data+2*i);
  
  printf( "Goodbye, cruel world from process %d of %d\n", rank, size );
  MPI_Finalize();
  return 0;
}

/************************************************************
This is a simple send/receive program in MPI
************************************************************

int main(argc,argv)
int argc;
char *argv[];
{
    int myid, numprocs;
    int tag,source,destination,count;
    int buffer;
    MPI_Status status;
 
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    tag=1234;
    source=0;
    destination=1;
    count=1;
    if(myid == source){
      buffer=5678;
      MPI_Send(&buffer,count,MPI_INT,destination,tag,MPI_COMM_WORLD);
      printf("processor %d  sent %d\n",myid,buffer);
    }
    if(myid == destination){
        MPI_Recv(&buffer,count,MPI_INT,source,tag,MPI_COMM_WORLD,&status);
        printf("processor %d  got %d\n",myid,buffer);
    }
    MPI_Finalize();
}

*/
