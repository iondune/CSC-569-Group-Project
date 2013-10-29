
#pragma once

#include "Application.h"
#include <mpi.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "MappedFile.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


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
        printf("\n");
        for (int i = 1; i < ProcessorCount; ++ i)
        {
            printf("Forking process %d\n", i);
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
                        printf(KGRN"Received result from %d"KNRM"\n", i);
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
            printf("Process forked with id %d\n", forkId);
            Children.push_back(forkId);
        }
        printf("\n");
        Profiler.End();
    }

    void ReceiveWorkFromSlaves()
    {
        Profiler.Start("Recv");
        printf("Waiting 5 seconds for children to catch up.\n");
        struct timeval WaitTime;
        WaitTime.tv_sec = 5;
        WaitTime.tv_usec = 0;
        int AttemptsCounter = 25;
        while (select(0, 0, 0, 0, & WaitTime) == -1 && AttemptsCounter-- > 0)
        {
            printf(KRED"Waiting failed! (%s) Making %d more attempts to wait"KNRM"\n", strerror(errno), AttemptsCounter);
        }

        for (int i = 0; i < Children.size(); ++ i)
        {
            printf(KCYN"Killing child %d"KNRM"\n", i+1);
            if (kill(Children[i], SIGKILL) == -1)
                printf("Failed! %s\n", strerror(errno));
        }
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
        printf(KGRN"Writing successfully completed."KNRM"\n");
    }

    void GetTheFuckOutOfHere()
    {
        printf(KMAG"Killing parent (%d) and self (%d)!"KNRM"\n", (int) getppid(), (int) getpid());
        kill(getppid(), SIGTERM);
        kill(getpid(), SIGKILL);
        printf("What!?!\n");
        exit(0);
    }

};
