
#pragma once

#include <string>
#include <vector>


class DataSet
{

public:

    void ParseFromString(char * const FileContents);
    void WriteToFile(std::string const & fileName);
    std::vector<int> MakeHistogram(float const Min, float const BinWidth);
    void WriteHistogramToFile(std::vector<int> const & Histogram, std::string const & fileName);
    void MakeSum(DataSet const & A, DataSet const & B);

    unsigned int Size() const;
    float operator[] (unsigned int const i) const;

    std::vector<float> Values;
    float Maximum;
};
