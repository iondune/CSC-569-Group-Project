#include "vector.h"
//#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "keyvalue.h"
#include "fileWriter.h"
#include <cmath>

#define CHUNK_SIZE 1024

char* serialReadfile(char *fname, int *sizeofFile);
int getBinCount(float min, float max, float width);
void printHistogram(int* bins, int size, const char *filename, float min, float max, float width);
void printVector(const vector<float>& vec, const char* filename);


int main(int narg, char **args)
{
   static float const BinWidth = 0.5f;
   static float const Min = -10.0f;
   static float const Min3 = -20.0f;
   MPI_Init(&narg,&args);

   int me,nprocs,i;
   MPI_Comm_rank(MPI_COMM_WORLD,&me);
   MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
   // printf("Let's start! My Rank is %d\n", me);

   if (narg <= 1) {
     if (me == 0) printf("Syntax: wordfreq file1 file2 ...\n");
     MPI_Abort(MPI_COMM_WORLD,1);
   }

   int fileSize1;
   int fileSize2;
   char *data1, *data2;
   if (me == 0)
   {
      data1 = serialReadfile(args[1], &fileSize1);
      data2 = serialReadfile(args[2], &fileSize2);
      int StringSize = strlen(data1);
      MPI_Bcast(& StringSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Bcast(data1, StringSize, MPI_CHAR, 0, MPI_COMM_WORLD);
      MPI_Bcast(data2, StringSize, MPI_CHAR, 0, MPI_COMM_WORLD);
   }
   else
   {
      int StringSize = 0;
      MPI_Bcast(& StringSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
      data1 = new char[StringSize];
      data2 = new char[StringSize];
      MPI_Bcast(data1, StringSize, MPI_CHAR, 0, MPI_COMM_WORLD);
      MPI_Bcast(data2, StringSize, MPI_CHAR, 0, MPI_COMM_WORLD);
   }

   MPI_Barrier(MPI_COMM_WORLD);
   double tstart = MPI_Wtime();

   Vector* a = Vector::from(data1, CHUNK_SIZE);
   Vector* b = Vector::from(data2, CHUNK_SIZE);

   MPI_Barrier(MPI_COMM_WORLD);

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

   a->bin(Min, max1, BinWidth, bins1);
   b->bin(Min, max2, BinWidth, bins2);
   c->bin(Min3, max3, BinWidth, bins3);

   MPI_Barrier(MPI_COMM_WORLD);

   double tstop = MPI_Wtime();

   if (me == 0) {
      printf("Time to process files: %g secs\n", tstop-tstart);

      // write results to files
      // old file output for debugging
      printHistogram(bins1, binCount1, "hist.a", Min, max1, BinWidth);
      printHistogram(bins2, binCount2, "hist.b", Min, max2, BinWidth);
      printHistogram(bins3, binCount3, "hist.c", Min3, max3, BinWidth);
      printVector(c->values(), "result.out");
   }

   MPI_Finalize();
}

char* serialReadfile(char *fname, int *sizeofFile) {
   struct stat stbuf;
   int flag = stat(fname,&stbuf);
   if (flag < 0) {
    printf("ERROR: Could not query file size\n");
    MPI_Abort(MPI_COMM_WORLD,1);
   }
   int filesize = stbuf.st_size;
   *sizeofFile = filesize;
   FILE *fp = fopen(fname,"r");
   char *text = new char[filesize+1];
   int nchar = fread(text,1,filesize,fp);
   text[nchar] = '\0';
   fclose(fp);
   //printf("data = %s",text);
   return text;
}

void printHistogram(int* bins, int size, const char* filename, float min,
                    float max, float width) {
   FILE* file = fopen(filename, "w");
   if (file == NULL) {
       fprintf(stderr, "Error opening file!\n");
       exit(1);
   }

   for (int i = 0; i < size; i++) {
      fprintf(file, "%d, %d\n", i, bins[i]);
   }
   fclose(file);
}

void printVector(const vector<float>& vec, const char *filename) {
   FILE *f = fopen(filename, "w");
   if (f == NULL) {
      printf("Error opening file!\n");
      exit(1);
   }

   int size = vec.size();
   for (int i = 0; i < size; i++){
      fprintf(f, "%.2f ", vec[i]);
   }
   fprintf(f, "\n");
   fclose(f);
}

int getBinCount(float min, float max, float width) {
  return ceil((max - min) / width);
}
