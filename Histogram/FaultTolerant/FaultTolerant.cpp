
// Libraries
#include <cstdlib>
#include <mpi.h>

#include "Application.h"
#include "MasterApplication.h"
#include "SlaveApplication.h"


int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: Serial <file1> <file2>");
        exit(EXIT_FAILURE);
    }

    MPI_Init(& argc, & argv);
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    int ProcessorId, ProcessorCount;
    MPI_Comm_size(MPI_COMM_WORLD, & ProcessorCount);
    MPI_Comm_rank(MPI_COMM_WORLD, & ProcessorId);

    Application * App;

    if (ProcessorId == 0)
        App = new MasterApplication(argv[1], argv[2]);
    else
        App = new SlaveApplication(ProcessorId);

    App->Run();


    MPI_Finalize();
    return 0;
}
