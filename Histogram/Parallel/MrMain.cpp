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
#define BIN_WIDTH 0.5f
#define MIN_VAL -10.0f
#define MIN_VAL_TIMES_TWO -20.0f

char* serialReadfile(char *fname, int *sizeofFile);
int getBinCount(float min, float max, float width);
void printHistogram(int* bins, int size, const char *filename);
void printVector(const vector<float>& vec, const char* filename);
int* initializeBins(Vector* vec, float min, int* binCount);

int main(int narg, char **args)
{
   MPI_Init(&narg,&args);

   int me,nprocs;
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

   int binCountA, binCountB, binCountC;
   int* binsA = initializeBins(a, MIN_VAL,           &binCountA);
   int* binsB = initializeBins(b, MIN_VAL,           &binCountB);
   int* binsC = initializeBins(c, MIN_VAL_TIMES_TWO, &binCountC);

   MPI_Barrier(MPI_COMM_WORLD);

   double tstop = MPI_Wtime();

   if (me == 0) {
      printf("Time to process files: %g secs\n", tstop-tstart);

      printHistogram(binsA, binCountA, "hist.a");
      printHistogram(binsB, binCountB, "hist.b");
      printHistogram(binsC, binCountC, "hist.c");
      printVector(c->values(), "result.out");
   }

   MPI_Finalize();
}

int* initializeBins(Vector* vec, float min, int* binCount) {
  float max = vec->max();
  (*binCount) = getBinCount(min, max, BIN_WIDTH);

  int binBytes = (*binCount) * sizeof(int);
  int* bins = (int*) malloc(binBytes);
  memset(bins, '\0', binBytes);

  vec->bin(min, max, BIN_WIDTH, bins);
  return bins;
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
   return text;
}

void printHistogram(int* bins, int size, const char* filename) {
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
