
// Standard Library
#include <iostream>
#include <vector>
#include <algorithm>
#include <cerrno>

// Unix shit
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

// MPI and Local
#include <mpi.h>
#include "hrt.h"
#include "DataSet.h"
#include "MappedFile.h"


/* Return 1 if the difference is negative, otherwise 0. */
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}

struct timeval tvBegin, tvEnd, tvDiff;

void startTimer()
{
    gettimeofday(&tvBegin, NULL);
}
void endTimer()
{
    gettimeofday(&tvEnd, NULL);
}
void printTimer()
{
    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);
}

int main(int argc, char * argv[])
{
    MPI_Init(& argc, & argv);
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    int ProcessorId, ProcessorCount;
    MPI_Comm_size(MPI_COMM_WORLD, & ProcessorCount);
    MPI_Comm_rank(MPI_COMM_WORLD, & ProcessorId);
    
    static char const * const ProcessorPrefix = 
        (ProcessorId ? "                            " : "");

    if (argc != 3)
    {
        fprintf(stderr, "usage: Serial <file1> <file2>");
        exit(EXIT_FAILURE);
    }


    ///////////////////
    // Read In Files //
    ///////////////////
    DataSet A, B;
    int Size = 0;
    if (ProcessorId == 0)
    {
        startTimer();
        MappedFile AFile(argv[1]), BFile(argv[2]);
        endTimer();
        printf("%sMap  %d  took ", ProcessorPrefix, ProcessorId); printTimer();
        
        startTimer();
        A.ParseFromString(AFile.Contents);
        B.ParseFromString(BFile.Contents);
        Size = A.Values.size();
        endTimer();
        printf("%sRead %d  took ", ProcessorPrefix, ProcessorId); printTimer();
    }


    ////////////////////////////////
    // Share Vectors Over Network //
    ////////////////////////////////
    startTimer();
    MPI_Bcast(& Size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (ProcessorId != 0)
    {
        A.Values.resize(Size);
        B.Values.resize(Size);
    }
    MPI_Bcast(& A.Values.front(), Size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(& B.Values.front(), Size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if (ProcessorId != 0)
    {
        B.Maximum = B.Values.back();
        A.Maximum = A.Values.back();
    }
    A.Values.erase(-- A.Values.end());
    B.Values.erase(-- B.Values.end());
    endTimer();
    printf("%s%s %d  took ", ProcessorPrefix, (ProcessorId ? "Recv" : "Send"), ProcessorId); printTimer();


    ///////////////////
    // Calculate Sum //
    ///////////////////
    startTimer();
    DataSet C;
    C.MakeSum(A, B);
    endTimer();
    printf("%sSum  %d  took ", ProcessorPrefix, ProcessorId); printTimer();

    C.Maximum = A.Maximum + B.Maximum;


    ////////////////////
    // Make Histogram //
    ////////////////////
    hrt_start();
    static float const BinWidth = 0.5f;
    static float const Min = -10.f;
    std::vector<int> const &
        HistA = A.MakeHistogram(Min, BinWidth),
        HistB = B.MakeHistogram(Min, BinWidth),
        HistC = C.MakeHistogram(Min*2, BinWidth);
    endTimer();
    printf("%sHist %d  took ", ProcessorPrefix, ProcessorId); printTimer();

    //////////////////
    // Write Output //
    //////////////////
    startTimer();
    if (ProcessorId == 0)
    {
        C.WriteToFile("result.out");
        A.WriteHistogramToFile(HistA, "hist.a");
        B.WriteHistogramToFile(HistB, "hist.b");
        C.WriteHistogramToFile(HistC, "hist.c");
    }
    endTimer();
    printf("%sWrit %d  took ", ProcessorPrefix, ProcessorId); printTimer();


    MPI_Finalize();
    return 0;
}
