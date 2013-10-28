
#include "Timer.h"

#include <cstdio>


int Timer::GetDiff()
{
    long int diff = (EndTime.tv_usec + 1000000 * EndTime.tv_sec) - (BeginTime.tv_usec + 1000000 * BeginTime.tv_sec);
    DiffTime.tv_sec = diff / 1000000;
    DiffTime.tv_usec = diff % 1000000;

    return (diff < 0);
}

void Timer::Start(std::string const & Label)
{
    CurrentLabel = Label;
    gettimeofday(& BeginTime, 0);
}

void Timer::End()
{
    gettimeofday(& EndTime, 0);
    GetDiff();
    printf("%s%6s  %d  took %ld.%06ld\n", Prefix.c_str(), CurrentLabel.c_str(),
        ProcessorId, DiffTime.tv_sec, DiffTime.tv_usec);
}

void Timer::SetPrefix(std::string const & prefix)
{
    Prefix = prefix;
}

void Timer::SetProcessorId(int const processorId)
{
    ProcessorId = processorId;
    SetPrefix(ProcessorId ? "                            " : "");
}
