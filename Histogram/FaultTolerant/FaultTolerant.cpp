
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cerrno>
using namespace std;

#include <sys/stat.h>
#include "hrt.h"
// #include <mpi.h>


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

static char * ReadWholeFile(const char * fileName)
{
    unsigned int s;
    char * contents;
    FILE * f;
    size_t bytesRead;
    int status;

    s = GetFileSize(fileName);
    if (! (contents = (char *) malloc(s + 1)))
    {
        fprintf(stderr, "Not enough memory.\n");
        exit(EXIT_FAILURE);
    }

    if (! (f = fopen(fileName, "r")))
    {
        fprintf(stderr, "Could not open '%s': %s.\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((bytesRead = fread(contents, sizeof(char), s, f)) != s)
    {
        fprintf (stderr, "Short read of '%s': expected %d bytes "
                 "but got %d: %s.\n", fileName, s, bytesRead, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if ((status = fclose(f)) != 0)
    {
        fprintf (stderr, "Error closing '%s': %s.\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return contents;
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
        
        char * const FileContents = ReadWholeFile(fileName.c_str());
        char * Token = strtok(FileContents, Tokenizer);
        Values.reserve(100000);
        while (Token)
        {
            float Value;
            if (sscanf(Token, "%f", & Value) == 1)
                Values.push_back(atof(Token));
            Token = strtok(0, Tokenizer);
        }
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
    
    void CalculateMaximum()
    {
        Maximum = * std::max_element(Values.begin(), Values.end());
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
    
    int Maximum;
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

    hrt_start();
    A.CalculateMaximum();
    B.CalculateMaximum();
    C.Maximum = A.Maximum + B.Maximum;
    hrt_stop();
    printf("Max   took %7s.\n", hrt_stringms());

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
