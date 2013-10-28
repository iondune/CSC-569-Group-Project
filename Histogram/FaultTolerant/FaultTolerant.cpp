
// Libraries
#include <cstdlib>
#include <mpi.h>

#include "Application/Application.h"
#include "Application/NetworkBenchmarkMasterApplication.h"
#include "Application/NetworkBenchmarkSlaveApplication.h"
#include "Application/FaultTolerantMasterApplication.h"
#include "Application/FaultTolerantSlaveApplication.h"
#include "Application/ParallelMasterApplication.h"
#include "Application/ParallelSlaveApplication.h"


int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "usage: Serial <file1> <file2> [mode] [options ...]");
        exit(EXIT_FAILURE);
    }

    std::string const DefaultMode = "FaultTolerant";
    std::string const Mode = (argc == 4 ? argv[3] : DefaultMode);

    MPI_Init(& argc, & argv);
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    int ProcessorId, ProcessorCount;
    MPI_Comm_size(MPI_COMM_WORLD, & ProcessorCount);
    MPI_Comm_rank(MPI_COMM_WORLD, & ProcessorId);

    Application * App = 0;

    if (Mode == "NetworkBenchmark")
    {
        if (ProcessorId == 0)
            App = new NetworkBenchmarkMasterApplication(argv[1], argv[2]);
        else
            App = new NetworkBenchmarkSlaveApplication(ProcessorId);
    }
    else if (Mode == "FaultTolerant")
    {
        if (ProcessorId == 0)
            App = new FaultTolerantMasterApplication(argv[1], argv[2], ProcessorCount);
        else
            App = new FaultTolerantSlaveApplication(ProcessorId);
    }
    else if (Mode == "Parallel")
    {
        if (ProcessorId == 0)
            App = new ParallelMasterApplication(argv[1], argv[2], ProcessorCount, 0, false);
        else
            App = new ParallelSlaveApplication(ProcessorId, false);
    }

    if (App)
        App->Run();


    MPI_Finalize();
    return 0;
}
