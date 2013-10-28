#include "vector.h"
//#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "keyvalue.h"
#include "fileWriter.h"
#include "MappedFile.h"
#include "DataSet.h"
#include <cmath>

char* serialReadfile(char *filename);
int getBinCount(float min, float max, float width);
void print(int* bins, int size, char *filename, float min, float max, float width, float *vec);

int main(int narg, char **args)
{
   static float const BinWidth = 0.5f;
   static float const Min = -10.0f;
   static float const Min3 = -20.0f;

   static float const numProcs = 5;
   static float const chunkSize = 512;


   MPI_Init(&narg,&args);

   int me,nprocs,i;
   MPI_Comm_rank(MPI_COMM_WORLD,&me);
   MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
   printf("Let's start! My Rank is %d\n", me);

   if (narg <= 1) {
     if (me == 0) printf("Syntax: wordfreq file1 file2 ...\n");
     MPI_Abort(MPI_COMM_WORLD,1);
   }

   if (me == 0) {
   	DataSet aVector;
   	DataSet bVector;

      MappedFile FileA(args[1]), FileB(args[2]);
      aVector.ParseFromString(FileA.Contents);
      bVector.ParseFromString(FileB.Contents);

      // aVector and bVector have Vectors of floats (member variable Value)
      // size of Vector found in .Size()
      // elements accessible with [] operator
   }

   char* data1 = serialReadfile(args[1]);
   char* data2 = serialReadfile(args[2]);
   //MapReduce *mr1 = new MapReduce(MPI_COMM_WORLD);
   //MapReduce *mr2 = new MapReduce(MPI_COMM_WORLD);
   //mr->verbosity = 2;
   //mr->timer = 1;
   //mr->memsize = 1;
   //mr->outofcore = 1;

   MPI_Barrier(MPI_COMM_WORLD);
   double tstart = MPI_Wtime();

   //read file for a
   //int nwords1 = mr1->map(1,&args[1],1,0,0,fileread,NULL);
   //int nfiles1 = mr1->mapfilecount;

   //read file for b
   //int nwords2 = mr2->map(1,&args[2],1,0,0,fileread,NULL);
   //int nfiles2 = mr2->mapfilecount;

   //MPI_Barrier(MPI_COMM_WORLD);

   //Vector v;
   //create a + b -> c
   Vector* a = Vector::from(data1, chunkSize, numProcs);
   Vector* b = Vector::from(data2, chunkSize, numProcs);
   

   float *sumsArray = (float *)malloc(1000000*sizeof(float));
   // Vector* c = a->add(b, sumsArray);
   Vector* c = a->add(b);

   MPI_Barrier(MPI_COMM_WORLD);

   float max1 = a->max();
   float max2 = b->max();
   float max3 = c->max();

   int binCount1 = getBinCount(Min, max1, BinWidth);
   int binCount2 = getBinCount(Min, max2, BinWidth);
   int binCount3 = getBinCount(Min3, max3, BinWidth);
   int* bins1 = (int *)malloc(binCount1 * sizeof(int));
   int* bins2 = (int *)malloc(binCount2 * sizeof(int));
   int* bins3 = (int *)malloc(binCount3 * sizeof(int));
   for (i=0;i<binCount1;i++) {
      *(bins1+i)=0;
   }
   for (i=0;i<binCount2;i++) {
      *(bins2+i)=0;
   }
   for (i=0;i<binCount3;i++) {
      *(bins3+i)=0;
   }

   MPI_Barrier(MPI_COMM_WORLD);

   a->bin(Min, max1, BinWidth, binCount1, bins1);
   b->bin(Min, max2, BinWidth, binCount2, bins2);
   c->bin(Min3, max3, BinWidth, binCount3, bins3);

   MPI_Barrier(MPI_COMM_WORLD);


   MPI_Barrier(MPI_COMM_WORLD);
   double tstop = MPI_Wtime();

   //delete mr1, mr2;
   char fname1[] = "hist.a";
   char fname2[] = "hist.b";
   char fname3[] = "hist.c";

   if (me == 0) {
      //printf("%d total words, %d unique words\n",nwords,nunique);
      printf("Time to process %d files on %d procs = %g (secs)\n",
	  2,1,tstop-tstart);

   	  // write results to files
      // should this be in here?
      //writeAllOutputFiles(sumsArray, bins1, bins2, bins3);
      // old file output for debugging
      print(bins1, binCount1, fname1, Min, max1, BinWidth, NULL);
      print(bins2, binCount2, fname2, Min, max2, BinWidth, NULL);
      print(bins3, binCount3, fname3, Min3, max3, BinWidth, sumsArray);
   }

   MPI_Finalize();
}

char* serialReadfile(char *fname) {
   struct stat stbuf;
   int flag = stat(fname,&stbuf);

   if (flag < 0) {
    printf("ERROR: Could not query file size\n");
    MPI_Abort(MPI_COMM_WORLD,1);
   }
   int filesize = stbuf.st_size;
   FILE *fp = fopen(fname,"r");
   char *text = new char[filesize+1];
   int nchar = fread(text,1,filesize,fp);
   text[nchar] = '\0';
   fclose(fp);
   //printf("data = %s",text);
   return text;   
}

void print(int* bins, int size, char *filename, float min, float max, float width, float* vec) {
   FILE *f = fopen(filename, "w");
   float low, up;
   int i;
   if (f == NULL)
   {
      printf("Error opening file!\n");
      exit(1);
   }

   for(i = 0; i < size;i++){
      low = min + width*i;
      up = low + width;
      fprintf(f, "%d, %d\n", i, bins[i]);
   }

   if (vec != NULL) {
      for(i = 0; i < 10000;i++){
         fprintf(f, "%.2f ",*(vec+i));
      }
   }
   fclose(f);
}

int getBinCount(float min, float max, float width) {
  return ceil((max - min) / width);
}

