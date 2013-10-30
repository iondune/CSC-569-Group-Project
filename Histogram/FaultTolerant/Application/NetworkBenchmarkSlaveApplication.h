
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
        MPI_Bcast(& VectorSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        A.Values.resize(VectorSize);
        B.Values.resize(VectorSize);
        MPI_Bcast(& A.Values.front(), VectorSize, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& B.Values.front(), VectorSize, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& A.Maximum, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& B.Maximum, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        Profiler.End();
    }

};
