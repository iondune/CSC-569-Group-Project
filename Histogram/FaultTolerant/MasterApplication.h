
#pragma once

#include "Application.h"


class MasterApplication : public Application
{

    std::string FileNameA, FileNameB;

public:

    MasterApplication(std::string const & fileNameA, std::string const & fileNameB);
    void Run();

    void ReadInFiles();
    void SendVectorsToSlaves();
    void CalculateSum();
    void MakeHistograms();
    void WriteOutputFiles();

};
