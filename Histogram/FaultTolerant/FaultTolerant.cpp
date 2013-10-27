
// Standard Library
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <limits>

// Unix shit
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

// MPI and HRT
#include <mpi.h>
#include "hrt.h"

/* Return 1 if the difference is negative, otherwise 0. */
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}


unsigned int GetFileSize(std::string const & fileName)
{
    struct stat sb;
    if (stat(fileName.c_str(), & sb) != 0)
    {
        fprintf(stderr, "'stat' failed for '%s': %s.\n", fileName.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sb.st_size;
}

template <typename T>
static T const Clamp(T const & v, T const & min, T const & max)
{
    return ((v >= max) ? max-1 : ((v < min) ? min : v));
}

class MappedFile
{

    int FileDescriptor;

public:

    MappedFile(std::string const & fileName)
    {
        Size = GetFileSize(fileName);
        FileDescriptor = open(fileName.c_str(), O_RDONLY, 0);
        if (FileDescriptor == -1)
        {
            fprintf(stderr, "Failed to open file '%s' for reading: %s\n", fileName.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }

        Contents = (char *) mmap(
            NULL,
            Size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE,
            FileDescriptor,
            0);

        if (Contents == MAP_FAILED)
        {
            fprintf(stderr, "Failed to map file '%s': %s\n", fileName.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    ~MappedFile()
    {
        munmap(Contents, Size);
        close(FileDescriptor);
    }

    char * Contents;
    unsigned int Size;
};

class DataSet
{

public:

    void ParseFromString(char * const FileContents)
    {
        static char const * const Tokenizer = " ";
        char * Token = strtok(FileContents, Tokenizer);
        Values.reserve(10000);
        Maximum = -std::numeric_limits<float>::max();
        while (Token)
        {
            float Value;
            if (sscanf(Token, "%f", & Value) == 1)
            {
                Values.push_back(atof(Token));
                Maximum = std::max(Maximum, Value);
            }
            Token = strtok(0, Tokenizer);
        }
        Values.push_back(Maximum);
    }

    void WriteToFile(std::string const & fileName)
    {
        FILE * outFile = fopen(fileName.c_str(), "w");
        if (! outFile)
        {
            fprintf(stderr, "Failed to open file '%s' for writing: %s\n", fileName.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < Values.size(); ++ i)
            fprintf(outFile, "%.2f ", Values[i]);
        fclose(outFile);
    }

    std::vector<int> MakeHistogram(float const Min, float const BinWidth)
    {
        int const BinCount = ceil((Maximum - Min) / BinWidth);

        std::vector<int> Histogram;
        Histogram.resize(BinCount);
        for (unsigned int i = 0; i < Values.size(); ++ i)
        {
            int index = Clamp((int) ((Values[i] - Min) / BinWidth), 0, BinCount);
            Histogram[Clamp(index, 0, BinCount)] ++;
        }

        return Histogram;
    }

    void WriteHistogramToFile(std::vector<int> const & Histogram, std::string const & fileName)
    {
        FILE * outFile = fopen(fileName.c_str(), "w");
        if (! outFile)
        {
            fprintf(stderr, "Failed to open file '%s' for writing: %s\n", fileName.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        for (unsigned int i = 0; i < Histogram.size(); ++ i)
            fprintf(outFile, "%d, %d\n", i, Histogram[i]);
        fclose(outFile);
    }

    void MakeSum(DataSet const & A, DataSet const & B)
    {
        if (A.Size() != B.Size())
        {
            std::cerr << "Vector sizes differ! " << A.Size() << " " << B.Size() << std::endl;
            exit(EXIT_FAILURE);
        }

        Values.resize(A.Size());
        for (unsigned int i = 0; i < A.Size(); ++ i)
            Values[i] = A[i] + B[i];
    }

    unsigned int Size() const
    {
        return Values.size();
    }

    float operator[] (unsigned int const i) const
    {
        return Values[i];
    }

    std::vector<float> Values;
    float Maximum;
};

struct timeval tvBegin, tvEnd, tvDiff;

void startTimer()
{
    gettimeofday(&tvBegin, NULL);
}
void endTimer()
{
    gettimeofday(&tvEnd, NULL);
}
void printTimer()
{
    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);
}

int main(int argc, char * argv[])
{
    MPI_Init(& argc, & argv);

    int ProcessorId, ProcessorCount;
    MPI_Comm_size(MPI_COMM_WORLD, & ProcessorCount);
    MPI_Comm_rank(MPI_COMM_WORLD, & ProcessorId);

    if (argc != 3)
    {
        fprintf(stderr, "usage: Serial <file1> <file2>");
        exit(EXIT_FAILURE);
    }

    startTimer();
    MappedFile AFile(argv[1]), BFile(argv[2]);
    endTimer();
    printf("Map  %d  took ", ProcessorId); printTimer();

    startTimer();
    DataSet A, B;
    int Size = 0;
    if (ProcessorId == 0)
    {
        A.ParseFromString(AFile.Contents);
        B.ParseFromString(BFile.Contents);
        Size = A.Values.size();
    }
    endTimer();
    printf("Read %d  took ", ProcessorId); printTimer();

    startTimer();
    MPI_Barrier(MPI_COMM_WORLD);
    endTimer();
    printf("Wait %d  took ", ProcessorId); printTimer();

    startTimer();
    MPI_Bcast(& Size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (ProcessorId != 0)
    {
        A.Values.resize(Size);
        B.Values.resize(Size);
    }
    MPI_Bcast(& A.Values.front(), Size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(& B.Values.front(), Size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if (ProcessorId != 0)
    {
        B.Maximum = B.Values.back();
        A.Maximum = A.Values.back();
    }
    A.Values.erase(-- A.Values.end());
    B.Values.erase(-- B.Values.end());
    endTimer();
    printf("Send %d  took ", ProcessorId); printTimer();

    startTimer();
    DataSet C;
    C.MakeSum(A, B);
    endTimer();
    printf("Sum  %d   took ", ProcessorId); printTimer();

    C.Maximum = A.Maximum + B.Maximum;

    hrt_start();
    static float const BinWidth = 0.5f;
    static float const Min = -10.f;
    std::vector<int> const &
        HistA = A.MakeHistogram(Min, BinWidth),
        HistB = B.MakeHistogram(Min, BinWidth),
        HistC = C.MakeHistogram(Min*2, BinWidth);
    endTimer();
    printf("Hist %d  took ", ProcessorId); printTimer();

    startTimer();
    if (ProcessorId == 0)
    {
        C.WriteToFile("result.out");
        A.WriteHistogramToFile(HistA, "hist.a");
        B.WriteHistogramToFile(HistB, "hist.b");
        C.WriteHistogramToFile(HistC, "hist.c");
    }
    endTimer();
    printf("Writ %d took ", ProcessorId); printTimer();
    
    MPI_Finalize();

    return 0;
}
