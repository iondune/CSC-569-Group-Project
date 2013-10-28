
#pragma once

#include "Application.h"
#include <unistd.h>


class ParallelSlaveApplication : public Application
{

    int N;
    bool DoHistSend;
    int BinCountA, BinCountB, BinCountC;

public:

    ParallelSlaveApplication(int ProcessorId, bool doHistSend)
    {
        Profiler.SetProcessorId(ProcessorId);
    }

    void Run()
    {
        DoSumForMaster();
        DoHistogramsForMaster();
    }

    void DoSumForMaster()
    {
        Profiler.Start("Recv");

        MPI_BCast(& N, 1, MPI_INT, 0, MPI_COMM_WORLD);
        A.Values.resize(N);
        B.Values.resize(N);

        MPI_Status Status;
        MPI_Recv(& A.Values.front(), N, MPI_FLOAT, 0, 123, MPI_COMM_WORLD, & Status);
        MPI_Recv(& B.Values.front(), N, MPI_FLOAT, 0, 234, MPI_COMM_WORLD, & Status);
        Profiler.Say("Received work!");
        C.MakeSum(A, B);
        Profiler.Say("Did work!");
        MPI_Send(& C.Values.front(), B, MPI_FLOAT, 0, 345, MPI_COMM_WORLD);
        Profiler.End();
    }

    void DoHistogramsForMaster()
    {
        if (DoHistSend)
        {
            MPI_Bcast(& BinCountA, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(& BinCountB, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(& BinCountC, 1, MPI_INT, 0, MPI_COMM_WORLD);

            static float const BinWidth = 0.5f;
            static float const Min = -10.f;

            HistA = A.MakeHistogram(Min, BinWidth, BinCountA);
            HistB = B.MakeHistogram(Min, BinWidth, BinCountB);
            HistC = C.MakeHistogram(Min*2, BinWidth, BinCountC);

            MPI_Send(& HistAWork.front(), BinCountA, MPI_FLOAT, i, 456, MPI_COMM_WORLD);
            MPI_Send(& HistBWork.front(), BinCountB, MPI_FLOAT, i, 457, MPI_COMM_WORLD);
            MPI_Send(& HistCWork.front(), BinCountC, MPI_FLOAT, i, 458, MPI_COMM_WORLD);
        }
    }

};
