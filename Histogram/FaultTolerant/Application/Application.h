
#pragma once

#include "DataSet.h"
#include "Timer.h"


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
