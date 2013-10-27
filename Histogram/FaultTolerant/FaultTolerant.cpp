
// Libraries
#include <cstdlib>
#include <mpi.h>

// MPI and Local
#include "DataSet.h"
#include "MappedFile.h"
#include "Timer.h"


int main(int argc, char * argv[])
{
    Timer timer;

    MPI_Init(& argc, & argv);
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    int ProcessorId, ProcessorCount;
    MPI_Comm_size(MPI_COMM_WORLD, & ProcessorCount);
    MPI_Comm_rank(MPI_COMM_WORLD, & ProcessorId);
    
    timer.SetProcessorId(ProcessorId);

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
        timer.Start("Map");
        MappedFile AFile(argv[1]), BFile(argv[2]);
        timer.End();
        
        timer.Start("Read");
        A.ParseFromString(AFile.Contents);
        B.ParseFromString(BFile.Contents);
        Size = A.Values.size();
        timer.End();
    }


    ////////////////////////////////
    // Share Vectors Over Network //
    ////////////////////////////////
    timer.Start("S+R");
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
    timer.End();


    ///////////////////
    // Calculate Sum //
    ///////////////////
    timer.Start("Sum");
    DataSet C;
    C.MakeSum(A, B);
    timer.End();

    C.Maximum = A.Maximum + B.Maximum;


    ////////////////////
    // Make Histogram //
    ////////////////////
    timer.Start("Hist");
    static float const BinWidth = 0.5f;
    static float const Min = -10.f;
    std::vector<int> const &
        HistA = A.MakeHistogram(Min, BinWidth),
        HistB = B.MakeHistogram(Min, BinWidth),
        HistC = C.MakeHistogram(Min*2, BinWidth);
    timer.End();


    //////////////////
    // Write Output //
    //////////////////
    timer.Start("Write");
    if (ProcessorId == 0)
    {
        C.WriteToFile("result.out");
        A.WriteHistogramToFile(HistA, "hist.a");
        B.WriteHistogramToFile(HistB, "hist.b");
        C.WriteHistogramToFile(HistC, "hist.c");
    }
    timer.End();


    MPI_Finalize();
    return 0;
}

