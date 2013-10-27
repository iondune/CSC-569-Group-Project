
// Libraries
#include <cstdlib>
#include <mpi.h>

// MPI and Local
#include "DataSet.h"
#include "MappedFile.h"
#include "Timer.h"


class Application
{

protected:

    Timer Profiler;
    DataSet A, B, C;
    std::vector<int> HistA, HistB, HistC;
    int VectorSize;

public:

    virtual void Run() = 0;

};

class MasterApplication : public Application
{

    std::string FileNameA, FileNameB;

public:

    MasterApplication(std::string const & fileNameA, std::string const & fileNameB)
    {
        FileNameA = fileNameA;
        FileNameB = fileNameB;
        Profiler.SetProcessorId(0);
    }

    void Run()
    {
        ReadInFiles();
        SendVectorsToSlaves();
        CalculateSum();
        MakeHistograms();
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
        Profiler.End();
    }

    void SendVectorsToSlaves()
    {
        Profiler.Start("Send");
        MPI_Bcast(& VectorSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& A.Values.front(), VectorSize, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& B.Values.front(), VectorSize, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& A.Maximum, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Bcast(& B.Maximum, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
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

};

class SlaveApplication : public Application
{

public:

    SlaveApplication(int ProcessorId)
    {
        Profiler.SetProcessorId(ProcessorId);
    }

    void Run()
    {
        ReceiveVectorsFromMaster();
    }

    void ReceiveVectorsFromMaster()
    {
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
