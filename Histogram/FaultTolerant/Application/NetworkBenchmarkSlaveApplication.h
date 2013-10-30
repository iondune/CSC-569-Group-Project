
#pragma once

#include "Application.h"


class NetworkBenchmarkSlaveApplication : public Application
{

public:

    NetworkBenchmarkSlaveApplication(int ProcessorId)
    {
        Profiler.SetProcessorId(ProcessorId);
    }

    void Run()
    {
        ReceiveVectorsFromMaster();
    }

    void ReceiveVectorsFromMaster()
    {
        MPI_Barrier(MPI_COMM_WORLD);
        Profiler.Start("Recv");
        unsigned int SizeA, SizeB;
        MPI_Bcast(& SizeA, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
        MPI_Bcast(& SizeB, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
        char * FileA = new char[SizeA], * FileB = new char[SizeB];
        MPI_Bcast(FileA, SizeA, MPI_CHAR, 0, MPI_COMM_WORLD);
        MPI_Bcast(FileB, SizeB, MPI_CHAR, 0, MPI_COMM_WORLD);
        Profiler.End();
    }

};
