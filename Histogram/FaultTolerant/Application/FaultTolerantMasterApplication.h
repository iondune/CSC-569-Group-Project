
#pragma once

#include "Application.h"
#include <mpi.h>
#include "MappedFile.h"


class FaultTolerantMasterApplication : public Application
{

    std::string FileNameA, FileNameB;
    int ProcessorCount;

public:

    FaultTolerantMasterApplication(std::string const & fileNameA, std::string const & fileNameB, int processorCount)
    {
        FileNameA = fileNameA;
        FileNameB = fileNameB;
        Profiler.SetProcessorId(0);
        ProcessorCount = processorCount;
    }

    void Run()
    {
        ReadInFiles();
        SendWorkToSlaves();
        CalculateSum();
        ReceiveWorkFromSlaves();
        MakeHistograms();
        WriteOutputFiles();
        //GetTheFuckOutOfHere();
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

    void SendWorkToSlaves()
    {
        Profiler.Start("Send");
        for (int i = 1; i < ProcessorCount; ++ i)
        {
            MPI_Send(& A.Values[i-1], 1, MPI_FLOAT, i, 1234, MPI_COMM_WORLD);
            MPI_Send(& B.Values[i-1], 1, MPI_FLOAT, i, 4321, MPI_COMM_WORLD);
        }
        Profiler.End();
    }

    void ReceiveWorkFromSlaves()
    {
        Profiler.Start("Recv");
        for (int i = 1; i < ProcessorCount; ++ i)
        {
            MPI_Status Status;
            float Result;
            MPI_Recv(& Result, 1, MPI_FLOAT, i, 9876, MPI_COMM_WORLD, & Status);
            printf("Received result from %d\n", i);
            if (C.Values[i-1] != Result)
                printf("ERROR! Worker results incorrect!\n");
            C.Values[i-1] = Result;
        }
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

    void GetTheFuckOutOfHere()
    {
        exit(0);
    }

};
