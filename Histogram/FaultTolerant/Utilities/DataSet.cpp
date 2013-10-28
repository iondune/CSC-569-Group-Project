
#include "DataSet.h"

#include <iostream>
#include <limits>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cmath>


template <typename T>
static T const Clamp(T const & v, T const & min, T const & max)
{
    return ((v >= max) ? max-1 : ((v < min) ? min : v));
}

void DataSet::ParseFromString(char * const FileContents)
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
}

void DataSet::WriteToFile(std::string const & fileName)
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

std::vector<int> DataSet::MakeHistogram(float const Min, float const BinWidth)
{
    return MakeHistogram(Min, BinWidth, ceil((Maximum - Min) / BinWidth));
}

std::vector<int> DataSet::MakeHistogram(float const Min, float const BinWidth, int const BinCount)
{
    std::vector<int> Histogram;
    Histogram.resize(BinCount);
    for (unsigned int i = 0; i < Values.size(); ++ i)
    {
        int index = Clamp((int) ((Values[i] - Min) / BinWidth), 0, BinCount);
        Histogram[Clamp(index, 0, BinCount)] ++;
    }

    return Histogram;
}

void DataSet::WriteHistogramToFile(std::vector<int> const & Histogram, std::string const & fileName)
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

void DataSet::MakeSum(DataSet const & A, DataSet const & B)
{
    MakeSum(A, B, 0, A.Size());
}

void DataSet::MakeSum(DataSet const & A, DataSet const & B, unsigned int const i0, unisnged int const i1)
{
    if (A.Size() != B.Size())
    {
        std::cerr << "Vector sizes differ! " << A.Size() << " " << B.Size() << std::endl;
        exit(EXIT_FAILURE);
    }
    if (i1 >= A.Size())
    {
        std::cerr << "Vector index out of range! " << i1 << " " << A.Size() << std::endl;
        exit(EXIT_FAILURE);
    }

    Values.resize(A.Size());
    for (unsigned int i = i0; i < i1; ++ i)
        Values[i] = A[i] + B[i];
}

unsigned int DataSet::Size() const
{
    return Values.size();
}

float DataSet::operator[] (unsigned int const i) const
{
    return Values[i];
}
