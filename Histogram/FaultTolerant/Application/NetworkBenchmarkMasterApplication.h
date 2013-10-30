
#pragma once

#include "Application.h"
#include <mpi.h>
#include "MappedFile.h"


class NetworkBenchmarkMasterApplication : public Application
{

    std::string FileNameA, FileNameB;

public:

    NetworkBenchmarkMasterApplication(std::string const & fileNameA, std::string const & fileNameB)
    {
        FileNameA = fileNameA;
        FileNameB = fileNameB;
        Profiler.SetProcessorId(0);
    }

    void Run()
    {
        ReadInFiles();
        SendVectorsToSlaves();
        CalculateSum();
        MakeHistograms();
        WriteOutputFiles();
    }

    void ReadInFiles()
    {
        Profiler.Start("Map");
        MappedFile FileA(FileNameA), FileB(FileNameB);
        Profiler.End();
        
        Profiler.Start("Read");
        A.ParseFromString(FileA.Contents);
        B.ParseFromString(FileB.Contents);
        VectorSize = A.Values.size();
        Profiler.End();
    }

    void SendVectorsToSlaves()
    {
        MPI_Barrier(MPI_COMM_WORLD);
        Profiler.Start("Send");
        MPI_Bcast(& VectorSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& A.Values.front(), VectorSize, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& B.Values.front(), VectorSize, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& A.Maximum, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& B.Maximum, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        Profiler.End();
    }

    void CalculateSum()
    {
        Profiler.Start("Sum");
        C.MakeSum(A, B);
        C.Maximum = A.Maximum + B.Maximum;
        Profiler.End();
    }

    void MakeHistograms()
    {
        Profiler.Start("Hist");
        static float const BinWidth = 0.5f;
        static float const Min = -10.f;
        HistA = A.MakeHistogram(Min, BinWidth);
        HistB = B.MakeHistogram(Min, BinWidth);
        HistC = C.MakeHistogram(Min*2, BinWidth);
        Profiler.End();
    }

    void WriteOutputFiles()
    {
        Profiler.Start("Write");
        C.WriteToFile("result.out");
        A.WriteHistogramToFile(HistA, "hist.a");
        B.WriteHistogramToFile(HistB, "hist.b");
        C.WriteHistogramToFile(HistC, "hist.c");
        Profiler.End();
    }

};
