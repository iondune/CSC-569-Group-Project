
#pragma once

#include "Application.h"


class SlaveApplication : public Application
{

public:

    SlaveApplication(int ProcessorId);
    void Run();

    void ReceiveVectorsFromMaster();

};
