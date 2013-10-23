
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <limits>
using namespace std;

#include <sys/stat.h>
#include "hrt.h"
// #include <mpi.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


unsigned int GetFileSize(char const * const fileName)
{
    struct stat sb;
    if (stat(fileName, & sb) != 0)
    {
        fprintf(stderr, "'stat' failed for '%s': %s.\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sb.st_size;
}

template <typename T>
static T const Clamp(T const & v, T const & min, T const & max)
{
    return ((v >= max) ? max-1 : ((v < min) ? min : v));
}

class DataSet
{

    std::vector<float> Values;

public:

    void ReadFromFile(std::string const & fileName)
    {
        static char const * const Tokenizer = " ";

        int fd = open(fileName.c_str(), O_RDONLY, 0);
        if (fd == -1)
        {
            fprintf(stderr, "Failed to open file '%s' for reading: %s\n", fileName.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }

        char * const FileContents = (char * const) mmap(
            NULL,
            GetFileSize(fileName.c_str()),
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE,
            fd,
            0);

        if (FileContents == MAP_FAILED)
        {
            fprintf(stderr, "Failed to map file '%s': %s\n", fileName.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
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
        munmap(FileContents, 4096);
        close(fd);
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

    float Maximum;
};


int main (int argc, char * argv[])
{
    // MPI_Init(& argc, & argv);

    // int ProcessorId, ProcessorCount;
    // MPI_Comm_size(MPI_COMM_WORLD, & ProcessorCount);
    // MPI_Comm_rank(MPI_COMM_WORLD, & ProcessorId);

    if (argc != 3)
    {
        fprintf(stderr, "usage: Serial <file1> <file2>");
        exit(EXIT_FAILURE);
    }

    DataSet A, B, C;

    hrt_start();
    A.ReadFromFile(argv[1]);
    B.ReadFromFile(argv[2]);
    hrt_stop();
    printf("Read  took %7s.\n", hrt_stringms());

    hrt_start();
    C.MakeSum(A, B);
    hrt_stop();
    printf("Sum   took %7s.\n", hrt_stringms());

    C.Maximum = A.Maximum + B.Maximum;

    hrt_start();
    static float const BinWidth = 0.5f;
    static float const Min = -10.f;
    std::vector<int> const &
        HistA = A.MakeHistogram(Min, BinWidth),
        HistB = B.MakeHistogram(Min, BinWidth),
        HistC = C.MakeHistogram(Min*2, BinWidth);
    hrt_stop();
    printf("Hist  took %7s.\n", hrt_stringms());

    hrt_start();
    C.WriteToFile("result.out");
    A.WriteHistogramToFile(HistA, "hist.a");
    B.WriteHistogramToFile(HistB, "hist.b");
    C.WriteHistogramToFile(HistC, "hist.c");
    hrt_stop();
    printf("Write took %7s.\n", hrt_stringms());

    return 0;
}
