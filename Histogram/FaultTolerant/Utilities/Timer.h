
#pragma once

#include <sys/time.h>
#include <string>


class Timer
{

    struct timeval BeginTime, EndTime, DiffTime;
    std::string CurrentLabel, Prefix;
    int ProcessorId;

    int GetDiff();

public:

    void Start(std::string const & Label);
    void End();

    void SetPrefix(std::string const & prefix);
    std::string const & GetPrefix() const;
    void SetProcessorId(int const processorId);
    void Say(std::string const & message);

};
