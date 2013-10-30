
#pragma once

#include "Application.h"
#include <mpi.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "MappedFile.h"


class FaultTolerantMasterApplication : public Application
{

    std::string FileNameA, FileNameB;
    int ProcessorCount;
    std::vector<int> Children, Pipes;

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
        SendWorkToSlaves();
        CalculateSum();
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
            int Pipe[2];
            pipe(Pipe);
            printf("Forking process %d\n", i);
            int forkId = fork();
            if (forkId == 0)
            {
                close(Pipe[0]);
                MPI_Send(& A.Values[i-1], 1, MPI_FLOAT, i, 1234, MPI_COMM_WORLD);
                MPI_Send(& B.Values[i-1], 1, MPI_FLOAT, i, 4321, MPI_COMM_WORLD);

                MPI_Status Status;
                float Result = 0;
                MPI_Recv(& Result, 1, MPI_FLOAT, i, 9876, MPI_COMM_WORLD, & Status);

                printf(KGRN"Received result from %d"KNRM"\n", i);
                if (write(Pipe[1], & Result, sizeof(Result)) == -1)
                    perror("write(Pipe)");
                close(Pipe[1]);
                exit(0);
            }
            close(Pipe[1]);
            printf("Process forked with id %d\n", forkId);
            Children.push_back(forkId);
            Pipes.push_back(Pipe[0]);
        }
        printf("\n");
        Profiler.End();
    }

    void CalculateSum()
    {
        Profiler.Start("Sum");
        C.MakeSum(A, B);
        C.Maximum = A.Maximum + B.Maximum;
        Profiler.End();
    }

    void ReceiveWorkFromSlaves()
    {
        Profiler.Start("Recv");
        printf("Waiting 5 seconds for children to catch up.\n");
        struct timeval WaitTime;
        WaitTime.tv_sec = 5;
        WaitTime.tv_usec = 0;
        int AttemptsCounter = ProcessorCount;
        while (select(0, 0, 0, 0, & WaitTime) == -1 && AttemptsCounter-- > 0)
        {
            printf(KRED"Found %d processes! (%s) Making %d more attempts to wait"KNRM"\n", ProcessorCount - AttemptsCounter, strerror(errno), AttemptsCounter);
            WaitTime.tv_sec = 5;
            WaitTime.tv_usec = 0;
        }

        printf("Waiting 3 seconds each for child to write.\n");
        for (int i = 0; i < Children.size(); ++ i)
        {
            WaitTime.tv_sec = 3;
            WaitTime.tv_usec = 0;
            fd_set FDs;
            FD_ZERO(& FDs);
            FD_SET(Pipes[i], & FDs);
            int Return = select(1+Pipes[i], & FDs, 0, 0, & WaitTime);
            if (Return == -1)
                perror("select(Pipe)");
            else
            {
                if (Return && FD_ISSET(Pipes[i], &FDs))
                {
                    float Result = -1;
                    if (read(Pipes[i], & Result, sizeof(Result)) == -1)
                        perror("read(PIPE)");
                    else if (C.Values[i] != Result)
                         printf("ERROR! Master results incorrect (%.2f), replacing with Worker result (%.2f)!\n", C.Values[i], Result);
                    else
                        printf(KCYN"Worker results verified %d"KNRM"\n", i);
                    C.Values[i] = Result;
                }
                else
                    printf(KRED"Worker not heard from: %d"KNRM"\n", i+1);
            }

            printf(KYEL"Killing child %d"KNRM"\n", i+1);
            close(Pipes[i]);
            if (kill(Children[i], SIGKILL) == -1)
                printf("Failed! %s\n", strerror(errno));
        }
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
