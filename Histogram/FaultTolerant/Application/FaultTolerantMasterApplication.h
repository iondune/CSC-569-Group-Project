
#pragma once

#include "Application.h"
#include <mpi.h>
#include <unistd.h>
#include <signal.h>
#include "MappedFile.h"


class FaultTolerantMasterApplication : public Application
{

    std::string FileNameA, FileNameB;
    int ProcessorCount;
    std::vector<int> Children;

public:

    FaultTolerantMasterApplication(std::string const & fileNameA, std::string const & fileNameB, int processorCount)
    {
        FileNameA = fileNameA;
        FileNameB = fileNameB;
        Profiler.SetProcessorId(0);
        ProcessorCount = processorCount;
    }

    void Run()
    {
        ReadInFiles();
        CalculateSum();
        SendWorkToSlaves();
        ReceiveWorkFromSlaves();
        MakeHistograms();
        WriteOutputFiles();
        GetTheFuckOutOfHere();
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

    void SendWorkToSlaves()
    {
        Profiler.Start("Send");
        for (int i = 1; i < ProcessorCount; ++ i)
        {
            int forkId = fork();
            if (forkId == 0)
            {
                MPI_Bsend(& A.Values[i-1], 1, MPI_FLOAT, i, 1234, MPI_COMM_WORLD);//, 0);
                MPI_Bsend(& B.Values[i-1], 1, MPI_FLOAT, i, 4321, MPI_COMM_WORLD);//, 0);
                //MPI_Isend(& A.Values[i-1], 1, MPI_FLOAT, i, 1234, MPI_COMM_WORLD, 0);
                //MPI_Isend(& B.Values[i-1], 1, MPI_FLOAT, i, 4321, MPI_COMM_WORLD, 0);

                MPI_Request Request;
                float Result = 0;
                MPI_Irecv(& Result, 1, MPI_FLOAT, i, 9876, MPI_COMM_WORLD, & Request);

                for (int t = 0; t < 10; ++ t)
                {
                    int Flag;
                    MPI_Test(& Request, & Flag, 0);
                    if (Flag)
                    {
                        printf("Received result from %d\n", i);
                        break;
                    }
                    usleep(10000);
                }
                if (C.Values[i-1] != Result)
                {
                    if (Result == 0)
                        printf("Worker was not fast enough, haha!\n");
                    else
                        printf("ERROR! Worker results incorrect!\n");
                }
                else
                    C.Values[i-1] = Result;

                exit(0);
            }
            Children.push_back(forkId);
        }
        Profiler.End();
    }

    void ReceiveWorkFromSlaves()
    {
        Profiler.Start("Recv");
        int x = 0;
        for (int j = 0; j < 100; ++ j)
            for (int i = 0; i < 10000000; ++ i)
                x ++;
        for (int i = 0; i < Children.size(); ++ i)
            kill(Children[i], SIGTERM);
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

    void GetTheFuckOutOfHere()
    {
        printf("See you later, assholes!\n");
        exit(0);
    }

};
