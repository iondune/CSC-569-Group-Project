
#pragma once

#include "Application.h"
#include <mpi.h>
#include <unistd.h>
#include <signal.h>
#include <algorithm>
#include <functional>
#include "MappedFile.h"


class ParallelMasterApplication : public Application
{

    static float const BinWidth = 0.5f;
    static float const Min = -10.f;

    std::string FileNameA, FileNameB;
    int ProcessorCount;
    int NperNode, Count;

public:

    ParallelMasterApplication(std::string const & fileNameA, std::string const & fileNameB, int processorCount)
    {
        FileNameA = fileNameA;
        FileNameB = fileNameB;
        Profiler.SetProcessorId(0);
        ProcessorCount = processorCount;
    }

    void Run()
    {
        SendFilesToSlaves();
        CalculateSum();
        ReceiveVectorsFromSlaves();
        MakeHistograms();
        WriteOutputFiles();
    }

    void SendFilesToSlaves()
    {
        MappedFile FileA(FileNameA), FileB(FileNameB);

        Profiler.Start("SendF");
        char * IteratorA = FileA.Contents, * IteratorB = FileB.Contents;
        int CountA = 0, CountB = 0;
        while (* IteratorA)
            if (* IteratorA ++ == ' ')
                CountA ++;
        while (* IteratorB)
            if (* IteratorB ++ == ' ')
                CountB ++;
        if (CountA != CountB)
        {
            printf(KRED"Vector A and B size mismatch: %d %d"KNRM"\n", CountA, CountB);
            exit(0);
        }
        Count = CountA;

        NperNode = Count / ProcessorCount;
        IteratorA = FileA.Contents;
        IteratorB = FileB.Contents;
        for (int i = 1; i < ProcessorCount; ++ i)
        {
            CountA = 0;
            CountB = 0;
            char * StartA = IteratorA, * StartB = IteratorB;
            while (* IteratorA && CountA < NperNode)
                if (* IteratorA ++ == ' ')
                    CountA ++;
            while (* IteratorB && CountB < NperNode)
                if (* IteratorB ++ == ' ')
                    CountB ++;
            int SentA = (int) (IteratorA - StartA);
            int SentB = (int) (IteratorB - StartB);
            char * SendA = new char[SentA];
            char * SendB = new char[SentB];
            memcpy(SendA, StartA, SentA);
            memcpy(SendB, StartB, SentB);
            SendA[SentA - 1] = SendB[SentB - 1] = 0;

            MPI_Send(& SentA, 1, MPI_INT, i, 111, MPI_COMM_WORLD);
            MPI_Send(& SentB, 1, MPI_INT, i, 112, MPI_COMM_WORLD);
            MPI_Send(SendA, SentA, MPI_CHAR, i, 113, MPI_COMM_WORLD);
            MPI_Send(SendB, SentB, MPI_CHAR, i, 114, MPI_COMM_WORLD);

            printf(KCYN"Sent (%d, %d) byte files to node %d (%d floats)"KNRM"\n", SentA, SentB, i, NperNode);
        }
        Profiler.End();

        Profiler.Start("Parse");
        A.ParseFromString(IteratorA);
        B.ParseFromString(IteratorB);
        Profiler.End();
    }

    void CalculateSum()
    {
        Profiler.Start("Sum");
        C.MakeSum(A, B);
        printf(KGRN"Calculated %d sums on host"KNRM"\n", C.Size());
        Profiler.End();
    }

    void ReceiveVectorsFromSlaves()
    {
        Profiler.Start("RecvV");
        for (int i = ProcessorCount - 1; i > 0; -- i)
        {
            MPI_Status Status;
            std::vector<float> Received(NperNode);
            MPI_Recv(& Received.front(), NperNode, MPI_FLOAT, i, 211, MPI_COMM_WORLD, & Status);
            A.Values.insert(A.Values.begin(), Received.begin(), Received.end());
            MPI_Recv(& Received.front(), NperNode, MPI_FLOAT, i, 212, MPI_COMM_WORLD, & Status);
            B.Values.insert(B.Values.begin(), Received.begin(), Received.end());
            MPI_Recv(& Received.front(), NperNode, MPI_FLOAT, i, 213, MPI_COMM_WORLD, & Status);
            C.Values.insert(C.Values.begin(), Received.begin(), Received.end());
        }
        C.CalculateMaximum();
        Profiler.End();
    }

    void MakeHistograms()
    {
        Profiler.Start("Hist");
        HistA = A.MakeHistogram(Min, BinWidth);
        HistB = B.MakeHistogram(Min, BinWidth);
        HistC = C.MakeHistogram(Min*2, BinWidth);
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
