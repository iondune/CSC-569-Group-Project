
#include "Timer.h"

#include <cstdio>


void Timer::start()
{
    gettimeofday(& beginTime, 0);
}

void Timer::end()
{
    gettimeofday(& endTime, 0);
}

void Timer::print()
{
    GetDiff();
    printf("%ld.%06ld\n", diffTime.tv_sec, diffTime.tv_usec);
}

