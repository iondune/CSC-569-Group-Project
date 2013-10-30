
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
        unsigned int SizeA = strlen(FileA.Contents), SizeB = strlen(FileB.Contents);
        Profiler.End();

        Profiler.Start("Count");
        char * Iterator = FileA.Contents;
        int ValueCount = 0;
        while (* Iterator)
            if (* Iterator ++ == ' ')
                ValueCount ++;
        printf("Values: %d\n", ValueCount);
        Profiler.End();

        MPI_Barrier(MPI_COMM_WORLD);
        Profiler.Start("Send");
        MPI_Bcast(& SizeA, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
        MPI_Bcast(& SizeB, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
        MPI_Bcast(FileA.Contents, SizeA, MPI_CHAR, 0, MPI_COMM_WORLD);
        MPI_Bcast(FileB.Contents, SizeB, MPI_CHAR, 0, MPI_COMM_WORLD);
        Profiler.End();
        
        Profiler.Start("Parse");
        A.ParseFromString(FileA.Contents);
        B.ParseFromString(FileB.Contents);
        VectorSize = A.Values.size();
        Profiler.End();
    }

    void SendVectorsToSlaves()
    {
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
