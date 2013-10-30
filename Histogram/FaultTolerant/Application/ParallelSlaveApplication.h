
#pragma once

#include "Application.h"
#include <unistd.h>


class ParallelSlaveApplication : public Application
{

    static float const BinWidth = 0.5f;
    static float const Min = -10.f;

public:

    ParallelSlaveApplication(int ProcessorId)
    {
        Profiler.SetProcessorId(ProcessorId);
    }

    void Run()
    {
        ReceiveFilesFromMaster();
        SendSumToMaster();
    }

    void ReceiveFilesFromMaster()
    {
        Profiler.Start("RecvF");
        MPI_Status Status;
        int SentA, SentB;
        MPI_Recv(& SentA, 1, MPI_INT, 0, 111, MPI_COMM_WORLD, & Status);
        MPI_Recv(& SentB, 1, MPI_INT, 0, 112, MPI_COMM_WORLD, & Status);
        char * SendA = new char[SentA];
        char * SendB = new char[SentB];
        MPI_Recv(SendA, SentA, MPI_CHAR, 0, 113, MPI_COMM_WORLD, & Status);
        MPI_Recv(SendB, SentB, MPI_CHAR, 0, 114, MPI_COMM_WORLD, & Status);
        Profiler.End();

        Profiler.Start("Parse");
        A.ParseFromString(SendA);
        B.ParseFromString(SendB);
        Profiler.End();
    }

    void SendSumToMaster()
    {
        Profiler.Start("Sum");
        C.MakeSum(A, B);
        printf("%sCalculated %d sums on slave.\n", Profiler.GetPrefix().c_str(), C.Size());
        Profiler.End();

        Profiler.Start("SendS");
        MPI_Send(& C.Values.front(), C.Size(), MPI_FLOAT, 0, 211, MPI_COMM_WORLD);
        Profiler.End();
    }

};
