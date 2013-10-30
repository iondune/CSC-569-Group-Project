
#pragma once

#include "DataSet.h"
#include "Timer.h"

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"


class Application
{

protected:

    Timer Profiler;
    DataSet A, B, C;
    std::vector<int> HistA, HistB, HistC;
    int VectorSize;

public:

    virtual void Run() = 0;

};
