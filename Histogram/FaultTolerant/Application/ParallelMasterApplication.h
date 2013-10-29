
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
    std::vector<int> Children;
    int N, Sent;
    int BinCountA, BinCountB, BinCountC;
    bool DoHistSend;

public:

    ParallelMasterApplication(std::string const & fileNameA, std::string const & fileNameB, int processorCount, int n, bool doHistSend)
    {
        FileNameA = fileNameA;
        FileNameB = fileNameB;
        Profiler.SetProcessorId(0);
        ProcessorCount = processorCount;
        N = n;
        Sent = 0;
        BinCountA = BinCountB = BinCountC = -1;
        DoHistSend = doHistSend;
    }

    void Run()
    {
        ReadInFiles();
        SendSumToSlaves();
        CalculateSum();
        ReceiveSumFromSlaves();
        MakeHistograms();
        ReceiveHistogramsFromSlaves();
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
        if (N == 0)
            N = VectorSize / ProcessorCount;
        Profiler.End();

        MPI_Barrier(MPI_COMM_WORLD);
    }

    void SendSumToSlaves()
    {
        Profiler.Start("Send");
        MPI_Bcast(& N, 1, MPI_INT, 0, MPI_COMM_WORLD);
        for (int i = 1; i < ProcessorCount; ++ i)
        {
            MPI_Send(& A.Values[Sent], N, MPI_FLOAT, i, 123, MPI_COMM_WORLD);
            MPI_Send(& B.Values[Sent], N, MPI_FLOAT, i, 234, MPI_COMM_WORLD);
            Sent += N;
        }
        Profiler.End();
    }

    void CalculateSum()
    {
        Profiler.Start("Sum");
        C.MakeSum(A, B, Sent, A.Size());
        C.Maximum = A.Maximum + B.Maximum;
        Profiler.End();
    }

    void ReceiveSumFromSlaves()
    {
        Profiler.Start("RecS");
        int Received = 0;
        for (int i = 1; i < ProcessorCount; ++ i)
        {
            MPI_Status Status;
            MPI_Recv(& C.Values[Received], N, MPI_FLOAT, i, 345, MPI_COMM_WORLD, & Status);
            Received += N;
        }
        BinCountA = A.GetBinCount(Min, BinWidth);
        BinCountB = B.GetBinCount(Min, BinWidth);
        BinCountC = C.GetBinCount(Min*2, BinWidth);
        if (DoHistSend)
        {
            MPI_Bcast(& BinCountA, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(& BinCountB, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(& BinCountC, 1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        Profiler.End();
    }

    void MakeHistograms()
    {
        Profiler.Start("Hist");
        if (DoHistSend)
        {
            HistA = A.MakeHistogram(Min, BinWidth, BinCountA, Sent, A.Size());
            HistB = B.MakeHistogram(Min, BinWidth, BinCountB, Sent, B.Size());
            HistC = C.MakeHistogram(Min*2, BinWidth, BinCountC, Sent, C.Size());
        }
        else
        {
            HistA = A.MakeHistogram(Min, BinWidth, BinCountA, 0, A.Size());
            HistB = B.MakeHistogram(Min, BinWidth, BinCountB, 0, B.Size());
            HistC = C.MakeHistogram(Min*2, BinWidth, BinCountC, 0, C.Size());
        }
        Profiler.End();
    }
    
    void ReceiveHistogramsFromSlaves()
    {
        if (DoHistSend)
        {
            Profiler.Start("RecH");
            std::vector<float> HistAWork, HistBWork, HistCWork;
            HistAWork.resize(BinCountA);
            HistBWork.resize(BinCountB);
            HistCWork.resize(BinCountC);
            for (int i = 1; i < ProcessorCount; ++ i)
            {
                MPI_Status Status;
                MPI_Recv(& HistAWork.front(), BinCountA, MPI_FLOAT, i, 456, MPI_COMM_WORLD, & Status);
                MPI_Recv(& HistBWork.front(), BinCountB, MPI_FLOAT, i, 457, MPI_COMM_WORLD, & Status);
                MPI_Recv(& HistCWork.front(), BinCountC, MPI_FLOAT, i, 458, MPI_COMM_WORLD, & Status);
                std::transform(HistAWork.begin(), HistAWork.end(), HistA.begin(), HistA.end(), std::plus<float>());
                std::transform(HistBWork.begin(), HistBWork.end(), HistB.begin(), HistB.end(), std::plus<float>());
                std::transform(HistCWork.begin(), HistCWork.end(), HistC.begin(), HistC.end(), std::plus<float>());
            }
            Profiler.End();
        }
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
