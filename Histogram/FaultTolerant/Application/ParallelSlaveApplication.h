
#pragma once

#include "Application.h"
#include <unistd.h>


class ParallelSlaveApplication : public Application
{

public:

    ParallelSlaveApplication(int ProcessorId)
    {
        Profiler.SetProcessorId(ProcessorId);
    }

    void Run()
    {
        DoWorkForMaster();
    }

    void DoWorkForMaster()
    {
        Profiler.Start("Recv");
        MPI_Status Status;
        float ValueA, ValueB;
        MPI_Recv(& ValueA, 1, MPI_FLOAT, 0, 1234, MPI_COMM_WORLD, & Status);
        MPI_Recv(& ValueB, 1, MPI_FLOAT, 0, 4321, MPI_COMM_WORLD, & Status);
        Profiler.Say("Received work!");
        float Sum = ValueA + ValueB;
        Profiler.Say("Did work!");
        MPI_Send(& Sum, 1, MPI_FLOAT, 0, 9876, MPI_COMM_WORLD);
        Profiler.End();
    }

};
