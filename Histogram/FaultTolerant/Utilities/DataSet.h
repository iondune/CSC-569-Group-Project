
#pragma once

#include <string>
#include <vector>


class DataSet
{

public:

    void ParseFromString(char * const FileContents);
    void WriteToFile(std::string const & fileName);
    int GetBinCount(float const Min, float const BinWidth);
    std::vector<int> MakeHistogram(float const Min, float const BinWidth);
    std::vector<int> MakeHistogram(float const Min, float const BinWidth, int const BinCount);
    std::vector<int> MakeHistogram(float const Min, float const BinWidth, int const BinCount, unsigned int i0, unsigned int i1);
    void WriteHistogramToFile(std::vector<int> const & Histogram, std::string const & fileName);
    void MakeSum(DataSet const & A, DataSet const & B);
    void MakeSum(DataSet const & A, DataSet const & B, unsigned int const i0, unsigned int const i1);

    unsigned int Size() const;
    float operator[] (unsigned int const i) const;

    std::vector<float> Values;
    float Maximum;
};
