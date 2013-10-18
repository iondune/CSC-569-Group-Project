
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cmath>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>


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
    
    std::vector<int> MakeHistogram(float const Min, float const BinWidth)
    {
        float const Max = * std::max_element(Values.begin(), Values.end());
        int const BinCount = ceil((Max - Min) / BinWidth);
        
        std::vector<int> Histogram;
        Histogram.resize(BinCount);
        for (unsigned int i = 0; i < Values.size(); ++ i)
        {
            int index = (int) ((Values[i] - Min) / BinWidth) % BinCount;
                //Clamp((int) ((Values[i] - Min) / BinWidth), 0, BinCount);
            Histogram[Clamp(index, 0, BinCount)] ++;
        }
        
        return Histogram;
    }
    
    void WriteHistogramToFile(float const Min, float const BinWidth, std::string const & fileName)
    {
        std::vector<int> Histogram = MakeHistogram(Min, BinWidth);
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
};


int main (int argc, char * argv[])
{
    static float const BinWidth = 0.5f;
    static float const Min = -10.f;
    
    if (argc != 3)
    {
        fprintf(stderr, "usage: Serial <file1> <file2>");
        exit(EXIT_FAILURE);
    }
    
    DataSet A, B, C;
    
    A.ReadFromFile(argv[1]);
    B.ReadFromFile(argv[2]);
    
    C.MakeSum(A, B);
    C.WriteToFile("result.out");
    
    A.WriteHistogramToFile(Min, BinWidth, "hist.a");
    B.WriteHistogramToFile(Min, BinWidth, "hist.b");
    C.WriteHistogramToFile(Min*2, BinWidth, "hist.c");
    
    return 0;
}
